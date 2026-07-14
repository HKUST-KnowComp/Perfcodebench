#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int stride = blockDim.x * gridDim.x;
    
    int sum = 0;
    while (i < n) {
        sum += input[i];
        i += stride;
    }
    
    sdata[tid] = sum;
    __syncthreads();
    
    for (unsigned int s = blockDim.x / 2; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    
    if (tid < 32) {
        int val = sdata[tid];
        val += __shfl_down_sync(0xffffffff, val, 16);
        val += __shfl_down_sync(0xffffffff, val, 8);
        val += __shfl_down_sync(0xffffffff, val, 4);
        val += __shfl_down_sync(0xffffffff, val, 2);
        val += __shfl_down_sync(0xffffffff, val, 1);
        sdata[tid] = val;
    }
    
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int blockSize = 256;
    int numBlocks = (n + blockSize - 1) / blockSize;
    if (numBlocks > 2048) numBlocks = 2048;
    if (numBlocks == 0) numBlocks = 1;
    
    size_t sharedMemSize = blockSize * sizeof(int);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<numBlocks, blockSize, sharedMemSize>>>(input, output, n);
    }
}