#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const float* __restrict__ input, float* partial, int n) {
    extern __shared__ float sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    float sum = 0.0f;
    // Grid-stride loop to accumulate multiple elements per thread
    for (int i = global_tid; i < n; i += gridDim.x * blockDim.x) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();
    // Tree reduction in shared memory
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        partial[blockIdx.x] = sdata[0];
    }
}

__global__ void final_reduce_kernel(const float* __restrict__ partial, float* result, int numBlocks) {
    extern __shared__ float sdata[];
    unsigned int tid = threadIdx.x;
    float val = 0.0f;
    if (tid < numBlocks) {
        val = partial[tid];
    }
    sdata[tid] = val;
    __syncthreads();
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        *result = sdata[0];
    }
}

} // anonymous namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    const int blockSize = 256;
    int numBlocks = (n + blockSize - 1) / blockSize;
    float* d_partial = nullptr;
    float* d_result = nullptr;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_result, sizeof(float));
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<numBlocks, blockSize, blockSize * sizeof(float)>>>(input, d_partial, n);
        final_reduce_kernel<<<1, blockSize, blockSize * sizeof(float)>>>(d_partial, d_result, numBlocks);
    }
    float actual;
    cudaMemcpy(&actual, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_result);
    return actual;
}
