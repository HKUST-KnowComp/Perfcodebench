#include "interface.h"
#include <cuda_runtime.h>

__global__ void reduce_block(const float* __restrict__ input,
                             float* __restrict__ partial,
                             int n)
{
    __shared__ float smem[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    float val = 0.0f;
    for (int i = idx; i < n; i += gridDim.x * blockDim.x)
        val += input[i];

    smem[tid] = val;
    __syncthreads();

    // warp-aggregated reduction
    for (int offset = blockDim.x / 2; offset >= 32; offset >>= 1) {
        if (tid < offset)
            smem[tid] += smem[tid + offset];
        __syncthreads();
    }

    val = smem[tid];
    for (int offset = 16; offset > 0; offset >>= 1)
        val += __shfl_down_sync(0xffffffff, val, offset);

    if (tid == 0)
        partial[blockIdx.x] = val;
}

__global__ void reduce_final(const float* __restrict__ partial,
                             float* __restrict__ result,
                             int blocks)
{
    __shared__ float smem[256];
    int tid = threadIdx.x;

    float val = 0.0f;
    for (int i = tid; i < blocks; i += blockDim.x)
        val += partial[i];

    smem[tid] = val;
    __syncthreads();

    for (int offset = blockDim.x / 2; offset >= 32; offset >>= 1) {
        if (tid < offset)
            smem[tid] += smem[tid + offset];
        __syncthreads();
    }

    val = smem[tid];
    for (int offset = 16; offset > 0; offset >>= 1)
        val += __shfl_down_sync(0xffffffff, val, offset);

    if (tid == 0)
        *result = val;
}

float reduce_f32(const float* input, int n, int iters)
{
    constexpr int block_size = 256;
    int blocks = (n + block_size - 1) / block_size;
    blocks = min(blocks, 1024); // cap grid size

    static float* d_partial = nullptr;
    static float* d_result  = nullptr;
    static bool allocated = false;

    if (!allocated) {
        cudaMalloc(&d_partial, sizeof(float) * 1024);
        cudaMalloc(&d_result,  sizeof(float));
        allocated = true;
    }

    float h_result = 0.0f;
    for (int i = 0; i < iters; ++i) {
        reduce_block<<<blocks, block_size>>>(input, d_partial, n);
        reduce_final<<<1, block_size>>>(d_partial, d_result, blocks);
    }

    cudaMemcpy(&h_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    return h_result;
}