#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_block_kernel(const int* input, int* output, int n) {
    extern __shared__ int shared[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int sum = 0;
    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }
    shared[tid] = sum;
    __syncthreads();

    // Reduction in shared memory
    for (int s = blockDim.x / 2; s > 32; s >>= 1) {
        if (tid < s) {
            shared[tid] += shared[tid + s];
        }
        __syncthreads();
    }

    // Warp-level reduction for the last 32 (or fewer) threads
    if (tid < 32) {
        volatile int* vshared = shared;
        if (blockDim.x > 32) vshared[tid] += vshared[tid + 32];
        vshared[tid] += vshared[tid + 16];
        vshared[tid] += vshared[tid + 8];
        vshared[tid] += vshared[tid + 4];
        vshared[tid] += vshared[tid + 2];
        vshared[tid] += vshared[tid + 1];
    }

    if (tid == 0) {
        atomicAdd(output, shared[0]);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 1024;
    const size_t smem = kBlock * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_block_kernel<<<grid, kBlock, smem>>>(input, output, n);
    }
}
