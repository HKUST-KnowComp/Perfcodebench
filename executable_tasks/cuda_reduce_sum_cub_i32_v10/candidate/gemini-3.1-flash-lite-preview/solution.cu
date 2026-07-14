#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle arbitrary n
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    // Block-level reduction using warp shuffles
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    if (tid % warpSize == 0) {
        sdata[tid / warpSize] = sum;
    }
    __syncthreads();

    // Final reduction of warp results
    if (tid < (blockDim.x + warpSize - 1) / warpSize) {
        sum = sdata[tid];
        for (int offset = ((blockDim.x + warpSize - 1) / warpSize) / 2; offset > 0; offset /= 2) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    const int grid_size = 1024;
    const int shared_mem = (block_size / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_kernel<<<grid_size, block_size, shared_mem>>>(input, output, n);
    }
}