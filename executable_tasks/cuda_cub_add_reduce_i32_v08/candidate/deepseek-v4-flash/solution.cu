#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;

__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[BLOCK_SIZE];

    int tid = threadIdx.x;
    int sum = 0;

    // Grid-stride loop: each thread processes multiple elements
    for (int i = blockIdx.x * BLOCK_SIZE + tid; i < n; i += gridDim.x * BLOCK_SIZE) {
        sum += input[i];
    }

    sdata[tid] = sum;
    __syncthreads();

    // Parallel reduction in shared memory
    for (int stride = BLOCK_SIZE / 2; stride > 0; stride >>= 1) {
        if (tid < stride) {
            sdata[tid] += sdata[tid + stride];
        }
        __syncthreads();
    }

    // First thread atomically adds block partial sum to output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        if (n > 0) {
            int grid = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
            reduce_kernel<<<grid, BLOCK_SIZE>>>(input, output, n);
        }
    }
}
