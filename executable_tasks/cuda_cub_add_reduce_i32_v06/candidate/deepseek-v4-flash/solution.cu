#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[256];  // blockDim.x = 256

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + tid;
    int local_sum = 0;

    // Grid-stride loop: each thread accumulates many elements
    while (i < n) {
        local_sum += input[i];
        i += gridDim.x * blockDim.x;
    }

    sdata[tid] = local_sum;
    __syncthreads();

    // In-warp tree reduction
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) {
        cudaMemset(output, 0, sizeof(int));
        return;
    }

    const int blockSize = 256;
    const int numBlocks = (n + blockSize - 1) / blockSize;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<numBlocks, blockSize>>>(input, output, n);
    }
}