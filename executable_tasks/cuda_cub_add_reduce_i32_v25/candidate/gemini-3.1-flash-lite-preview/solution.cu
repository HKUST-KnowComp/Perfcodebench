#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * (blockDim.x * 2) + threadIdx.x;
    
    int sum = 0;
    if (i < n) sum += input[i];
    if (i + blockDim.x < n) sum += input[i + blockDim.x];

    sdata[tid] = sum;
    __syncthreads();

    for (unsigned int s = blockDim.x / 2; s > 32; s >>= 1) {
        if (tid < s) sdata[tid] += sdata[tid + s];
        __syncthreads();
    }

    if (tid < 32) {
        volatile int* vdata = sdata;
        for (int s = 32; s > 0; s >>= 1) vdata[tid] += vdata[tid + s];
        if (tid == 0) atomicAdd(output, vdata[0]);
    }
}
}

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads = 256;
    const int blocks = (n + (threads * 2) - 1) / (threads * 2);
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads, threads * sizeof(int)>>>(input, output, n);
    }
}