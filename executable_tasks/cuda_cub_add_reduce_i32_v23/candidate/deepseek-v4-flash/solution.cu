#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int blockStart = blockIdx.x * blockDim.x;
    int idx = blockStart + tid;

    // Load data into shared memory with boundary check
    sdata[tid] = (idx < n) ? input[idx] : 0;
    __syncthreads();

    // Tree reduction in shared memory
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // First thread of each block atomically adds block sum to global output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int blockSize = 256;
    const int gridSize = (n + blockSize - 1) / blockSize;
    const int sharedMemSize = blockSize * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<gridSize, blockSize, sharedMemSize>>>(input, output, n);
    }
}
