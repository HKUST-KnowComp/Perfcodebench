#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    unsigned int start = blockIdx.x * blockDim.x + tid;
    unsigned int stride = blockDim.x * gridDim.x;
    int sum = 0;
    for (unsigned int i = start; i < (unsigned int)n; i += stride) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();
    for (unsigned int s = blockDim.x >> 1; s > 0; s >>= 1) {
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
        for (int iter = 0; iter < iters; ++iter) {
            *output = 0;
        }
        return;
    }
    const int blockSize = 256;
    const int gridSize = (n + blockSize - 1) / blockSize;
    const size_t shmemSize = blockSize * sizeof(int);
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<gridSize, blockSize, shmemSize>>>(input, output, n);
    }
}
