#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[kBlockSize];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    // Coalesced global load with boundary check
    sdata[tid] = (idx < n) ? input[idx] : 0;
    __syncthreads();

    // Shared memory tree reduction
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // First thread of each block atomically adds the block sum to output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int grid = (n + kBlockSize - 1) / kBlockSize;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        block_reduce_kernel<<<grid, kBlockSize>>>(input, output, n);
    }
}
