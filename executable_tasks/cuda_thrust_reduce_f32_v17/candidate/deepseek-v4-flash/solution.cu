#include "interface.h"
#include <cuda_runtime.h>

namespace {
    constexpr int blockSize = 256;

    __global__ void reduce_kernel(const float* input, float* partialSums, int n) {
        __shared__ float sdata[blockSize];
        int tid = threadIdx.x;
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        double sum = 0.0;
        for (int i = idx; i < n; i += gridDim.x * blockDim.x) {
            sum += input[i];
        }
        sdata[tid] = static_cast<float>(sum);
        __syncthreads();

        for (int s = blockDim.x / 2; s > 0; s >>= 1) {
            if (tid < s) {
                sdata[tid] += sdata[tid + s];
            }
            __syncthreads();
        }
        if (tid == 0) {
            partialSums[blockIdx.x] = sdata[0];
        }
    }

    __global__ void final_reduce_kernel(const float* partialSums, float* output, int n) {
        __shared__ float sdata[blockSize];
        int tid = threadIdx.x;
        double sum = 0.0;
        for (int i = tid; i < n; i += blockDim.x) {
            sum += partialSums[i];
        }
        sdata[tid] = static_cast<float>(sum);
        __syncthreads();

        for (int s = blockDim.x / 2; s > 0; s >>= 1) {
            if (tid < s) {
                sdata[tid] += sdata[tid + s];
            }
            __syncthreads();
        }
        if (tid == 0) {
            *output = sdata[0];
        }
    }
}

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    int numBlocks = (n + blockSize - 1) / blockSize;

    float* d_partialSums = nullptr;
    float* d_output = nullptr;
    cudaMalloc(&d_partialSums, numBlocks * sizeof(float));
    cudaMalloc(&d_output, sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<numBlocks, blockSize>>>(input, d_partialSums, n);
        final_reduce_kernel<<<1, blockSize>>>(d_partialSums, d_output, numBlocks);
    }

    float result = 0.0f;
    cudaMemcpy(&result, d_output, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_partialSums);
    cudaFree(d_output);

    return result;
}
