#include "interface.h"
#include <cuda_runtime.h>

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float sdata[256];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x * 2 + threadIdx.x;
    unsigned int gridSize = blockDim.x * 2 * gridDim.x;
    
    float sum = 0.0f;
    while (i < n) {
        sum += input[i];
        if (i + blockDim.x < n) sum += input[i + blockDim.x];
        i += gridSize;
    }
    
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    if (tid % warpSize == 0) {
        sdata[tid / warpSize] = sum;
    }
    __syncthreads();
    
    if (tid < warpSize) {
        sum = (tid < blockDim.x / warpSize) ? sdata[tid] : 0.0f;
        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float result = 0.0f;
    
    int threads = 256;
    int blocks = (n + threads * 2 - 1) / (threads * 2);
    if (blocks > 1024) blocks = 1024;
    if (blocks < 1) blocks = 1;
    
    for (int i = 0; i < iters; ++i) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<blocks, threads>>>(input, d_out, n);
    }
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return result;
}