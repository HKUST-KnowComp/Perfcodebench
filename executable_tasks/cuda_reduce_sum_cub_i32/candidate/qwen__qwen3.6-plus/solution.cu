#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[256];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * 512 + threadIdx.x;
    unsigned int gridSize = 512 * gridDim.x;
    
    int mySum = 0;
    while (i < n) {
        mySum += input[i];
        if (i + 256 < n) {
            mySum += input[i + 256];
        }
        i += gridSize;
    }
    sdata[tid] = mySum;
    __syncthreads();

    for (unsigned int s = 128; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid < 32) {
        int val = sdata[tid];
        #pragma unroll
        for (int offset = 16; offset > 0; offset >>= 1) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    int blocks = (n + 511) / 512;
    if (blocks > 2048) blocks = 2048;
    if (blocks == 0) blocks = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, 256>>>(input, output, n);
    }
}
