#include "interface.h"
#include <cuda_runtime.h>
#include <cstddef>

namespace {

constexpr int BLOCK = 256;          // power of two, good occupancy on all architectures
constexpr int MAX_BLOCKS = 1024;    // upper bound to limit scratch size

__global__ void reduce_kernel(const float* __restrict__ in, float* __restrict__ partial, int n) {
    __shared__ float smem[BLOCK];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + tid;

    // grid-stride load and first add
    float sum = 0.0f;
    while (i < n) {
        sum += in[i];
        i += gridDim.x * blockDim.x;
    }
    smem[tid] = sum;
    __syncthreads();

    // shared-memory reduction
    for (int offset = blockDim.x / 2; offset > 32; offset >>= 1) {
        if (tid < offset) smem[tid] += smem[tid + offset];
        __syncthreads();
    }

    // warp shuffle reduction (last warp)
    if (tid < 32) {
        float val = smem[tid];
        for (int offset = 32 / 2; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        if (tid == 0) partial[blockIdx.x] = val;
    }
}

__global__ void reduce_single_block(const float* __restrict__ in, float* __restrict__ out, int n) {
    __shared__ float smem[BLOCK];
    int tid = threadIdx.x;
    float sum = 0.0f;
    for (int i = tid; i < n; i += blockDim.x)
        sum += in[i];
    smem[tid] = sum;
    __syncthreads();

    for (int offset = blockDim.x / 2; offset > 32; offset >>= 1) {
        if (tid < offset) smem[tid] += smem[tid + offset];
        __syncthreads();
    }

    if (tid < 32) {
        float val = smem[tid];
        for (int offset = 32 / 2; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        if (tid == 0) *out = val;
    }
}

} // anonymous namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_part = nullptr;
    float* d_out  = nullptr;
    cudaMalloc(&d_part, MAX_BLOCKS * sizeof(float));
    cudaMalloc(&d_out,  sizeof(float));

    int num_blocks = (n + BLOCK - 1) / BLOCK;
    if (num_blocks > MAX_BLOCKS) num_blocks = MAX_BLOCKS;

    for (int iter = 0; iter < iters; ++iter) {
        if (num_blocks > 1) {
            reduce_kernel<<<num_blocks, BLOCK>>>(input, d_part, n);
            reduce_single_block<<<1, BLOCK>>>(d_part, d_out, num_blocks);
        } else {
            reduce_single_block<<<1, BLOCK>>>(input, d_out, n);
        }
    }

    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_part);
    cudaFree(d_out);
    return result;
}