#include "interface.h"
#include <cuda_runtime.h>

namespace {

template<unsigned int blockSize>
__device__ void warpReduce(volatile float* sdata, unsigned int tid) {
    if (blockSize >= 64) sdata[tid] += sdata[tid + 32];
    if (blockSize >= 32) sdata[tid] += sdata[tid + 16];
    if (blockSize >= 16) sdata[tid] += sdata[tid + 8];
    if (blockSize >= 8) sdata[tid] += sdata[tid + 4];
    if (blockSize >= 4) sdata[tid] += sdata[tid + 2];
    if (blockSize >= 2) sdata[tid] += sdata[tid + 1];
}

template<unsigned int blockSize>
__global__ void reduce_kernel(const float* input, float* output, int n) {
    __shared__ float sdata[blockSize];
    
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * (blockSize * 2) + threadIdx.x;
    unsigned int gridSize = blockSize * 2 * gridDim.x;
    
    float sum = 0.0f;
    while (i < n) {
        sum += input[i];
        if (i + blockSize < n)
            sum += input[i + blockSize];
        i += gridSize;
    }
    sdata[tid] = sum;
    __syncthreads();
    
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } __syncthreads(); }
    if (blockSize >= 128) { if (tid < 64) { sdata[tid] += sdata[tid + 64]; } __syncthreads(); }
    
    if (tid < 32) warpReduce<blockSize>(sdata, tid);
    
    if (tid == 0) output[blockIdx.x] = sdata[0];
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    const int blockSize = 256;
    const int numBlocks = min((n + blockSize * 2 - 1) / (blockSize * 2), 1024);
    
    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));
    
    float result = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<blockSize><<<numBlocks, blockSize>>>(input, d_partial, n);
        reduce_kernel<blockSize><<<1, blockSize>>>(d_partial, d_out, numBlocks);
    }
    
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    
    cudaFree(d_partial);
    cudaFree(d_out);
    
    return result;
}