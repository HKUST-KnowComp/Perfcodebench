#include "interface.h"
#include <cuda_runtime.h>

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int sum = 0;
    // Grid-stride loop: each thread sums its assigned elements
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();
    // Tree reduction in shared memory
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int blockSize = 256;
    // Use a fixed grid of 1024 blocks; idle threads simply add zero
    const int gridSize = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_kernel<<<gridSize, blockSize, blockSize * sizeof(int)>>>(input, output, n);
    }
}