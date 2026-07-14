#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int nthreads = blockDim.x;
    int stride = nthreads * gridDim.x;
    
    int sum = 0;
    for (int i = bid * nthreads + tid; i < n; i += stride) {
        sum += input[i];
    }
    
    sdata[tid] = sum;
    __syncthreads();
    
    for (int s = nthreads / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        int blocks = (n + 255) / 256;
        if (blocks > 1024) blocks = 1024;
        if (blocks == 0) blocks = 1;
        reduce_kernel<<<blocks, 256, 256 * sizeof(int)>>>(input, output, n);
    }
}
