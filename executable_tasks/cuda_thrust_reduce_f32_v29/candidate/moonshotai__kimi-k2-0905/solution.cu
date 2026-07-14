#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

__global__ void reduceBlock(const float * __restrict__ in, float * __restrict__ out, int n) {
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int blockSize = blockDim.x;
    const int gridSize = gridDim.x * blockSize * 128;   // 128 elems per thread
    const int stride = gridDim.x * blockSize;

    float sum = 0.0f;
    for (int i = bid * blockSize * 128 + tid; i < n; i += stride) {
        #pragma unroll 128
        for (int k = 0; k < 128; ++k) {
            int idx = i + k * blockSize;
            if (idx < n) sum += in[idx];
        }
    }

    __shared__ float smem[256];
    smem[tid] = sum;
    __syncthreads();

    // warp-aggregated reduction
    if (tid < 128) smem[tid] += smem[tid + 128]; __syncthreads();
    if (tid <  64) smem[tid] += smem[tid +  64]; __syncthreads();
    if (tid <  32) {
        volatile float *v = smem;
        v[tid] += v[tid + 32];
        v[tid] += v[tid + 16];
        v[tid] += v[tid +  8];
        v[tid] += v[tid +  4];
        v[tid] += v[tid +  2];
        v[tid] += v[tid +  1];
    }
    if (tid == 0) out[blockIdx.x] = smem[0];
}

__global__ void reduceFinal(const float * __restrict__ partial, float * __restrict__ result, int numBlocks) {
    __shared__ float smem[256];
    int tid = threadIdx.x;
    float sum = 0.0f;
    for (int i = tid; i < numBlocks; i += blockDim.x) sum += partial[i];
    smem[tid] = sum;
    __syncthreads();

    if (tid < 128) smem[tid] += smem[tid + 128]; __syncthreads();
    if (tid <  64) smem[tid] += smem[tid +  64]; __syncthreads();
    if (tid <  32) {
        volatile float *v = smem;
        v[tid] += v[tid + 32];
        v[tid] += v[tid + 16];
        v[tid] += v[tid +  8];
        v[tid] += v[tid +  4];
        v[tid] += v[tid +  2];
        v[tid] += v[tid +  1];
    }
    if (tid == 0) *result = smem[0];
}

float reduce_f32(const float* input, int n, int iters) {
    const int blockSize = 256;
    const int numBlocks = std::min(1024, (n + blockSize * 128 - 1) / (blockSize * 128));

    float *d_partial = nullptr;
    float *d_out     = nullptr;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_out,     sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduceBlock<<<numBlocks, blockSize>>>(input, d_partial, n);
        reduceFinal<<<1, blockSize>>>(d_partial, d_out, numBlocks);
    }

    float h_result;
    cudaMemcpy(&h_result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_out);
    return h_result;
}