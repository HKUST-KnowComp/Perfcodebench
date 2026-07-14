#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#define BLOCK_SIZE 256

__global__ void reduce_kernel(const float* input, float* output, int n)
{
    __shared__ float sdata[BLOCK_SIZE];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockIdx.x * blockDim.x + threadIdx.x;
    // Corrected index calculation for grid
    i = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;
    while (i < n) {
        sum += input[i];
        i += gridDim.x * blockDim.x;
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
        output[blockIdx.x] = sdata[0];
    }
}

float reduce_f32(const float* input, int n, int iters) {
    if (n == 0) return 0.0f;

    int blockSize = BLOCK_SIZE;
    int gridSize = (n + blockSize - 1) / blockSize;
    if (gridSize > 1024) gridSize = 1024; // Cap grid size for the second pass

    float *d_block_sums = nullptr;
    cudaMalloc(&d_block_sums, gridSize * sizeof(float));

    float final_result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        // First pass: reduce to gridSize elements
        reduce_kernel<<<gridSize, blockSize>>>(input, d_block_sums, n);
        
        // Second pass: reduce the block sums
        // For simplicity and performance in this specific harness, 
        // we can do the final small reduction on CPU or a small kernel.
        // Given the iters loop, we keep it on GPU.
        float h_block_sums[1024];
        cudaMemcpy(h_block_sums, d_block_sums, gridSize * sizeof(float), cudaMemcpyDeviceToHost);
        
        float iter_sum = 0.0f;
        for(int i = 0; i < gridSize; ++i) iter_sum += h_block_sums[i];
        final_result = iter_sum;
    }

    cudaFree(d_block_sums);
    return final_result;
}