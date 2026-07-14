#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

#define BLOCK_SIZE 256
#define MAX_BLOCKS 1024

__global__ void reduce_kernel(const float * __restrict__ input,
                              float * __restrict__ blockSums,
                              int n)
{
    __shared__ float smem[BLOCK_SIZE];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    // grid-stride load
    float sum = 0.0f;
    for (int i = idx; i < n; i += blockDim.x * gridDim.x)
        sum += input[i];
    smem[tid] = sum;
    __syncthreads();

    // warp-level reduction
    for (int offset = warpSize/2; offset > 0; offset /= 2)
        smem[tid] += __shfl_down_sync(0xFFFFFFFF, smem[tid], offset);

    // write warp 0 result to shared
    if (tid % warpSize == 0)
        smem[tid / warpSize] = smem[tid];
    __syncthreads();

    // final reduction in warp 0
    if (tid < warpSize)
    {
        float val = (tid < (BLOCK_SIZE/warpSize)) ? smem[tid] : 0.0f;
        for (int offset = warpSize/2; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        if (tid == 0)
            blockSums[blockIdx.x] = val;
    }
}

__global__ void reduce_single_block(const float * __restrict__ partials,
                                    float * __restrict__ out,
                                    int numParts)
{
    __shared__ float smem[BLOCK_SIZE];
    int tid = threadIdx.x;
    float sum = 0.0f;
    for (int i = tid; i < numParts; i += blockDim.x)
        sum += partials[i];
    smem[tid] = sum;
    __syncthreads();

    for (int offset = blockDim.x/2; offset > 0; offset /= 2)
    {
        if (tid < offset)
            smem[tid] += smem[tid + offset];
        __syncthreads();
    }
    if (tid == 0)
        *out = smem[0];
}

float reduce_f32(const float *input, int n, int iters)
{
    static float *d_blockSums = nullptr;
    static float *d_out = nullptr;
    static int allocatedMaxBlocks = 0;

    int numBlocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (numBlocks > MAX_BLOCKS) numBlocks = MAX_BLOCKS;

    if (allocatedMaxBlocks < numBlocks)
    {
        if (d_blockSums) cudaFree(d_blockSums);
        if (d_out) cudaFree(d_out);
        cudaMalloc(&d_blockSums, numBlocks * sizeof(float));
        cudaMalloc(&d_out, sizeof(float));
        allocatedMaxBlocks = numBlocks;
    }

    float result = 0.0f;
    for (int iter = 0; iter < iters; ++iter)
    {
        reduce_kernel<<<numBlocks, BLOCK_SIZE>>>(input, d_blockSums, n);
        reduce_single_block<<<1, BLOCK_SIZE>>>(d_blockSums, d_out, numBlocks);
        if (iter == iters - 1)
            cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    }
    return result;
}