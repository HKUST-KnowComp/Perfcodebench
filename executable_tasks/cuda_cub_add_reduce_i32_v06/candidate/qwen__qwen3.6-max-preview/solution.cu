#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[256];
    int tid = threadIdx.x;
    int sum = 0;
    
    // Grid-stride loop for coalesced global memory reads
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    
    sdata[tid] = sum;
    __syncthreads();
    
    // Parallel reduction in shared memory
    if (tid < 128) sdata[tid] += sdata[tid + 128]; __syncthreads();
    if (tid < 64)  sdata[tid] += sdata[tid + 64];  __syncthreads();
    
    // Final warp reduction using shuffles (avoids shared memory bank conflicts)
    if (tid < 32) {
        int val = sdata[tid];
        val += __shfl_down_sync(0xffffffff, val, 16);
        val += __shfl_down_sync(0xffffffff, val, 8);
        val += __shfl_down_sync(0xffffffff, val, 4);
        val += __shfl_down_sync(0xffffffff, val, 2);
        val += __shfl_down_sync(0xffffffff, val, 1);
        if (tid == 0) atomicAdd(output, val);
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