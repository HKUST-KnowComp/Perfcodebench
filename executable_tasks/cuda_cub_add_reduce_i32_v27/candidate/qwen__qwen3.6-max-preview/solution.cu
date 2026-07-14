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
    if (tid < 64) sdata[tid] += sdata[tid + 64]; __syncthreads();
    
    if (tid < 32) {
        int warp_sum = sdata[tid];
        warp_sum += __shfl_down_sync(0xffffffff, warp_sum, 16);
        warp_sum += __shfl_down_sync(0xffffffff, warp_sum, 8);
        warp_sum += __shfl_down_sync(0xffffffff, warp_sum, 4);
        warp_sum += __shfl_down_sync(0xffffffff, warp_sum, 2);
        warp_sum += __shfl_down_sync(0xffffffff, warp_sum, 1);
        if (tid == 0) atomicAdd(output, warp_sum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int blocks = (n + 255) / 256;
    if (blocks > 1024) blocks = 1024;
    if (blocks < 1) blocks = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, 256>>>(input, output, n);
    }
}