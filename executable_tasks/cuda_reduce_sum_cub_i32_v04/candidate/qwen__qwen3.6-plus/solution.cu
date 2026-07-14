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
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    size_t shared_mem_size = kBlock * sizeof(int);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock, shared_mem_size>>>(input, output, n);
    }
}
