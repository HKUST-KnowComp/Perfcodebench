#include "interface.h"
#include <cuda_runtime.h>

__global__ void reduce_warp_shfl(const float* __restrict__ input, float* __restrict__ blockOut, int n)
{
    __shared__ float smem[32];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    float val = 0.0f;
    for (; idx < n; idx += gridDim.x * blockDim.x)
        val += input[idx];

    // Warp-level reduction using shuffle
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);

    if (tid % warpSize == 0)
        smem[tid / warpSize] = val;
    __syncthreads();

    // Final reduction in first warp
    if (tid < 32)
    {
        val = smem[tid];
        for (int offset = 16; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        if (tid == 0)
            blockOut[blockIdx.x] = val;
    }
}

__global__ void reduce_final(const float* __restrict__ partial, float* __restrict__ result, int numBlocks)
{
    __shared__ float smem[32];

    int tid = threadIdx.x;
    float val = (tid < numBlocks) ? partial[tid] : 0.0f;

    for (int offset = warpSize / 2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);

    if (tid % warpSize == 0)
        smem[tid / warpSize] = val;
    __syncthreads();

    if (tid < 32)
    {
        val = smem[tid];
        for (int offset = 16; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        if (tid == 0)
            *result = val;
    }
}

float reduce_f32(const float* input, int n, int iters)
{
    const int blockSize = 128;
    const int numBlocks = std::min((n + blockSize - 1) / blockSize, 1024);

    float *d_partial = nullptr, *d_result = nullptr;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_result, sizeof(float));

    float h_result = 0.0f;
    for (int i = 0; i < iters; ++i)
    {
        reduce_warp_shfl<<<numBlocks, blockSize>>>(input, d_partial, n);
        reduce_final<<<1, 32>>>(d_partial, d_result, numBlocks);
    }

    cudaMemcpy(&h_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_result);
    return h_result;
}