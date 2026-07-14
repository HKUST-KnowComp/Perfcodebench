#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

// ------------------------------------------------------------------
// Tunable parameters
// ------------------------------------------------------------------
static constexpr int BLOCK_SIZE      = 256;          // threads per block
static constexpr int WARP_SIZE       = 32;
static constexpr int WARPS_PER_BLOCK = BLOCK_SIZE / WARP_SIZE;
static constexpr int UNROLL          = 8;            // grid-stride unroll

// ------------------------------------------------------------------
// Device reduction kernel
// ------------------------------------------------------------------
__global__ void reduce_kernel(const float* __restrict__ input,
                              float* __restrict__ blockSums,
                              int n)
{
    // ----------------------------------------------------------------
    // 1. Grid-stride load and warp-level partial reduction
    // ----------------------------------------------------------------
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int gtid = bid * blockDim.x + tid;
    const int gridStride = gridDim.x * blockDim.x * UNROLL;

    float sum = 0.0f;
    #pragma unroll
    for (int i = gtid; i < n; i += gridStride) {
        #pragma unroll
        for (int u = 0; u < UNROLL; ++u) {
            int idx = i + u * gridDim.x * blockDim.x;
            if (idx < n) sum += input[idx];
        }
    }

    // ----------------------------------------------------------------
    // 2. Warp shuffle reduction
    // ----------------------------------------------------------------
    const int lane = tid & 31;
    const int warp = tid >> 5;

    #pragma unroll
    for (int offset = WARP_SIZE / 2; offset > 0; offset /= 2)
        sum += __shfl_down_sync(0xffffffff, sum, offset);

    // ----------------------------------------------------------------
    // 3. Block-level reduction (only warp leaders participate)
    // ----------------------------------------------------------------
    __shared__ float warpSums[WARPS_PER_BLOCK];
    if (lane == 0) warpSums[warp] = sum;
    __syncthreads();

    if (warp == 0) {
        sum = (tid < WARPS_PER_BLOCK) ? warpSums[tid] : 0.0f;
        #pragma unroll
        for (int offset = WARPS_PER_BLOCK / 2; offset > 0; offset /= 2)
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        if (tid == 0) blockSums[bid] = sum;
    }
}

// ------------------------------------------------------------------
// Final single-block reduction of partial sums
// ------------------------------------------------------------------
__global__ void final_reduce_kernel(float* __restrict__ blockSums,
                                    int numBlocks,
                                    float* __restrict__ d_result)
{
    __shared__ float smem[BLOCK_SIZE];
    int tid = threadIdx.x;
    float sum = 0.0f;
    for (int i = tid; i < numBlocks; i += blockDim.x)
        sum += blockSums[i];
    smem[tid] = sum;
    __syncthreads();

    // power-of-two reduction
    for (int offset = blockDim.x / 2; offset > 0; offset /= 2) {
        if (tid < offset) smem[tid] += smem[tid + offset];
        __syncthreads();
    }
    if (tid == 0) *d_result = smem[0];
}

// ------------------------------------------------------------------
// Host-side persistent allocation and launch logic
// ------------------------------------------------------------------
float reduce_f32(const float* input, int n, int iters)
{
    // 1. Persistent device allocations
    static float* d_blockSums = nullptr;
    static float* d_result    = nullptr;
    static int    maxBlocks   = 0;

    const int numBlocks = (n + BLOCK_SIZE * UNROLL - 1) / (BLOCK_SIZE * UNROLL);
    if (numBlocks > maxBlocks) {
        cudaFree(d_blockSums);
        cudaMalloc(&d_blockSums, numBlocks * sizeof(float));
        maxBlocks = numBlocks;
    }
    if (!d_result) cudaMalloc(&d_result, sizeof(float));

    // 2. Repeated reduction launches
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<numBlocks, BLOCK_SIZE>>>(input, d_blockSums, n);
        final_reduce_kernel<<<1, BLOCK_SIZE>>>(d_blockSums, numBlocks, d_result);
    }

    // 3. Copy back result
    float h_result;
    cudaMemcpy(&h_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    return h_result;
}