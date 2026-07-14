#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int tid = threadIdx.x;
    int sum = 0;
    
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        sum += input[idx];
    }

    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    __shared__ int warp_sums[32];
    if (tid % 32 == 0) {
        warp_sums[tid / 32] = sum;
    }
    __syncthreads();

    if (tid < 32) {
        int ws = (tid < (blockDim.x + 31) / 32) ? warp_sums[tid] : 0;
        #pragma unroll
        for (int offset = 16; offset > 0; offset >>= 1) {
            ws += __shfl_down_sync(0xffffffff, ws, offset);
        }
        if (tid == 0) {
            atomicAdd(output, ws);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads = 256;
    int blocks = (n + threads - 1) / threads;
    if (blocks > 1024) blocks = 1024;
    if (blocks == 0) blocks = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads>>>(input, output, n);
    }
}
