#include "interface.h"
#include <cuda_runtime.h>

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    extern __shared__ float sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x * 2 + threadIdx.x;
    unsigned int gridSize = blockDim.x * 2 * gridDim.x;
    float sum = 0.0f;

    while (i < n) {
        sum += input[i];
        if (i + blockDim.x < n) sum += input[i + blockDim.x];
        i += gridSize;
    }
    sdata[tid] = sum;
    __syncthreads();

    for (unsigned int s = blockDim.x / 2; s > 32; s >>= 1) {
        if (tid < s) sdata[tid] += sdata[tid + s];
        __syncthreads();
    }
    if (tid < 32) {
        float val = sdata[tid];
        val += __shfl_down_sync(0xffffffff, val, 16);
        val += __shfl_down_sync(0xffffffff, val, 8);
        val += __shfl_down_sync(0xffffffff, val, 4);
        val += __shfl_down_sync(0xffffffff, val, 2);
        val += __shfl_down_sync(0xffffffff, val, 1);
        sdata[tid] = val;
    }
    if (tid == 0) output[blockIdx.x] = sdata[0];
}

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    int blockSize = 256;
    int numBlocks = (n + blockSize * 2 - 1) / (blockSize * 2);
    if (numBlocks > 1024) numBlocks = 1024;
    size_t sharedMemSize = blockSize * sizeof(float);
    
    float* d_block_sums = nullptr;
    cudaMalloc(&d_block_sums, numBlocks * sizeof(float));
    
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<numBlocks, blockSize, sharedMemSize>>>(input, d_block_sums, n);
    }
    
    float* h_block_sums = new float[numBlocks];
    cudaMemcpy(h_block_sums, d_block_sums, numBlocks * sizeof(float), cudaMemcpyDeviceToHost);
    float total = 0.0f;
    for (int i = 0; i < numBlocks; ++i) total += h_block_sums[i];
    
    cudaFree(d_block_sums);
    delete[] h_block_sums;
    return total;
}