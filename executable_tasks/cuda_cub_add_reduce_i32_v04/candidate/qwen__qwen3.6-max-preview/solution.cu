#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[32];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    int sum = 0;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    
    #pragma unroll
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    if (tid % 32 == 0) {
        sdata[tid / 32] = sum;
    }
    __syncthreads();
    
    if (tid < 32) {
        int val = (tid < blockDim.x / 32) ? sdata[tid] : 0;
        #pragma unroll
        for (int offset = 16; offset > 0; offset /= 2) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (tid == 0) {
            atomicAdd(output, val);
        }
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