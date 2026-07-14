#include "interface.h"
#include <cuda_runtime.h>

__global__ void parallel_reduce_kernel(const float* input, float* output, int n) {
    extern __shared__ float sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;
    while (i < n) {
        sum += input[i];
        i += blockDim.x * gridDim.x;
    }
    sdata[tid] = sum;
    __syncthreads();

    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

float reduce_f32(const float* input, int n, int iters) {
    float* d_out;
    cudaMalloc(&d_out, sizeof(float));
    
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    if (blocks > 1024) blocks = 1024;

    float result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        parallel_reduce_kernel<<<blocks, threads, threads * sizeof(float)>>>(input, d_out, n);
        if (iter == iters - 1) {
            cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
        }
    }

    cudaFree(d_out);
    return result;
}