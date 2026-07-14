#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int gridSize = blockDim.x * gridDim.x;
    
    int sum = 0;
    while (i < n) {
        sum += input[i];
        i += gridSize;
    }
    sdata[tid] = sum;
    __syncthreads();
    
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
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    if (blocks > 1024) blocks = 1024;
    size_t shared_mem_size = threads * sizeof(int);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads, shared_mem_size>>>(input, output, n);
    }
}
