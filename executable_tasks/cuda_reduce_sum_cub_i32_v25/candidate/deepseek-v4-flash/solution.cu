#include "interface.h"

#include <cuda_runtime.h>

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];  // dynamic shared memory, size = blockDim.x * sizeof(int)

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    // grid-stride loop to accumulate multiple elements per thread
    int sum = 0;
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();

    // tree reduction in shared memory
    for (int s = blockDim.x / 2; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // warp shuffle reduction for the final 32 elements
    if (tid < 32) {
        int warp_sum = sdata[tid];
        for (int offset = 16; offset > 0; offset >>= 1) {
            warp_sum += __shfl_down_sync(0xffffffff, warp_sum, offset);
        }
        // only thread 0 of the first warp performs the global atomic add
        if (tid == 0) {
            atomicAdd(output, warp_sum);
        }
    }
}

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;

    // Use at most 4096 blocks to avoid launching too many blocks for small n
    if (grid > 4096) grid = 4096;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock, kBlock * sizeof(int)>>>(input, output, n);
    }
}
