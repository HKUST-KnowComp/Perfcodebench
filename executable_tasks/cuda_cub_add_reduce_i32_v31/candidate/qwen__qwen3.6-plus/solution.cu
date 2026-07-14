#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 256;

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[BLOCK_SIZE];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int gridSize = blockDim.x * gridDim.x;
    
    int my_sum = 0;
    while (i < n) {
        my_sum += input[i];
        i += gridSize;
    }
    sdata[tid] = my_sum;
    __syncthreads();
    
    for (unsigned int s = BLOCK_SIZE / 2; s > 32; s >>= 1) {
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
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (num_blocks > 1024) num_blocks = 1024;
    if (num_blocks == 0) num_blocks = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, n);
    }
}
