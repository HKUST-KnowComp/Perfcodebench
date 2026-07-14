#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    int sum = 0;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    
    sdata[tid] = sum;
    __syncthreads();
    
    if (tid < 128) sdata[tid] += sdata[tid + 128]; __syncthreads();
    if (tid < 64)  sdata[tid] += sdata[tid + 64];  __syncthreads();
    
    if (tid < 32) {
        int val = sdata[tid];
        val += __shfl_down_sync(0xffffffff, val, 16);
        val += __shfl_down_sync(0xffffffff, val, 8);
        val += __shfl_down_sync(0xffffffff, val, 4);
        val += __shfl_down_sync(0xffffffff, val, 2);
        val += __shfl_down_sync(0xffffffff, val, 1);
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int blockSize = 256;
    int numBlocks = (n + blockSize - 1) / blockSize;
    if (numBlocks > 512) numBlocks = 512;
    if (numBlocks < 1) numBlocks = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<numBlocks, blockSize>>>(input, output, n);
    }
}