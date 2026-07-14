#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const float* input, float* output, int n) {
    __shared__ float sdata[256];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int gridSize = blockDim.x * gridDim.x;
    
    float sum = 0.0f;
    while (i < n) {
        sum += input[i];
        i += gridSize;
    }
    sdata[tid] = sum;
    __syncthreads();
    
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}
}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    
    int blockSize = 256;
    int numBlocks = (n + blockSize - 1) / blockSize;
    if (numBlocks > 1024) numBlocks = 1024;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<numBlocks, blockSize>>>(input, d_out, n);
    }
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}