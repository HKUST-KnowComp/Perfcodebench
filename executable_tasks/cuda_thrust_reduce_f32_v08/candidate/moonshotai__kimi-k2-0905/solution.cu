#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

namespace {

constexpr int BLOCK = 256;
constexpr int ELEMS_PER_THREAD = 128;

__global__ void reduceBlockKernel(const float* __restrict__ in, float* __restrict__ partial, int n) {
    int tid = threadIdx.x;
    int gtid = blockIdx.x * blockDim.x * ELEMS_PER_THREAD + tid;
    float sum = 0.0f;
    #pragma unroll
    for (int i = 0; i < ELEMS_PER_THREAD; ++i) {
        int idx = gtid + i * blockDim.x;
        if (idx < n) sum += in[idx];
    }
    __shared__ float smem[BLOCK];
    smem[tid] = sum;
    __syncthreads();
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) smem[tid] += smem[tid + offset];
        __syncthreads();
    }
    if (tid == 0) partial[blockIdx.x] = smem[0];
}

__global__ void reduceFinalKernel(float* __restrict__ partial, int numBlocks) {
    int tid = threadIdx.x;
    __shared__ float smem[BLOCK];
    float val = (tid < numBlocks) ? partial[tid] : 0.0f;
    smem[tid] = val;
    __syncthreads();
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) smem[tid] += smem[tid + offset];
        __syncthreads();
    }
    if (tid == 0) partial[0] = smem[0];
}

} // namespace

float reduce_f32(const float* input, int n, int iters) {
    static float* d_partial = nullptr;
    static int  maxBlocks = 0;
    if (!d_partial) {
        int blocks = (n + BLOCK * ELEMS_PER_THREAD - 1) / (BLOCK * ELEMS_PER_THREAD);
        cudaMalloc(&d_partial, sizeof(float) * std::max(blocks, 1));
        maxBlocks = blocks;
    }
    for (int iter = 0; iter < iters; ++iter) {
        int blocks = (n + BLOCK * ELEMS_PER_THREAD - 1) / (BLOCK * ELEMS_PER_THREAD);
        reduceBlockKernel<<<blocks, BLOCK>>>((const float*)input, d_partial, n);
        reduceFinalKernel<<<1, BLOCK>>>(d_partial, blocks);
    }
    float h_result;
    cudaMemcpy(&h_result, d_partial, sizeof(float), cudaMemcpyDeviceToHost);
    return h_result;
}