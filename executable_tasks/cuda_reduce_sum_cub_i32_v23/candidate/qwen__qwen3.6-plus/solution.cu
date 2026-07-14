#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warp_reduce_sum(int val) {
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int stride = blockDim.x * gridDim.x;

    int sum = 0;
    for (int i = bid * blockDim.x + tid; i < n; i += stride) {
        sum += input[i];
    }

    sum = warp_reduce_sum(sum);

    if (tid % 32 == 0) {
        sdata[tid / 32] = sum;
    }
    __syncthreads();

    int num_warps = (blockDim.x + 31) / 32;
    if (tid < num_warps) {
        sum = sdata[tid];
        sum = warp_reduce_sum(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    
    size_t shared_mem = kBlock * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock, shared_mem>>>(input, output, n);
    }
}