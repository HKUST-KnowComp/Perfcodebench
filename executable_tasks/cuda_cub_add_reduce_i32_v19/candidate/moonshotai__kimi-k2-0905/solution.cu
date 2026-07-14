#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdint>

namespace {

constexpr int BLOCK_THREADS = 256;
constexpr int ITEMS_PER_THREAD = 16;                 // 128-bit vector loads → 4 int32 per thread
constexpr int TILE_SIZE = BLOCK_THREADS * ITEMS_PER_THREAD;

__global__ void reduce_kernel(const int* __restrict__ input,
                              int* __restrict__ blockSums,
                              int n, int iters)
{
    using BlockReduce = cub::BlockReduce<int, BLOCK_THREADS>;
    __shared__ typename BlockReduce::TempStorage temp;

    const int tid = threadIdx.x;
    const int blockOff = blockIdx.x * TILE_SIZE;
    const int gridStride = gridDim.x * TILE_SIZE;

    for (int iter = 0; iter < iters; ++iter)
    {
        int threadSum = 0;

        // Vectorized loads with 128-bit alignment
        const int4* vecIn = reinterpret_cast<const int4*>(input);
        int idx = blockOff + tid * 4;
        #pragma unroll
        for (; idx + 3 < n; idx += gridStride)
        {
            int4 v = vecIn[idx / 4];
            threadSum += v.x + v.y + v.z + v.w;
        }
        // Tail
        idx = blockOff + tid;
        for (; idx < n; idx += gridDim.x * BLOCK_THREADS)
            threadSum += input[idx];

        int blockSum = BlockReduce(temp).Sum(threadSum);
        if (tid == 0) atomicAdd(&blockSums[blockIdx.x + iter * gridDim.x], blockSum);
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters)
{
    int device;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);
    const int blocks = prop.multiProcessorCount; // occupancy-tuned launch

    int* dBlockSums;
    cudaMalloc(&dBlockSums, sizeof(int) * blocks * iters);
    cudaMemset(dBlockSums, 0, sizeof(int) * blocks * iters);

    reduce_kernel<<<blocks, BLOCK_THREADS>>>(input, dBlockSums, n, iters);

    // Final reduction of block sums on device
    int64_t hostSum = 0;
    int* hBlockSums = new int[blocks * iters];
    cudaMemcpy(hBlockSums, dBlockSums, sizeof(int) * blocks * iters, cudaMemcpyDeviceToHost);
    for (int i = 0; i < blocks * iters; ++i) hostSum += hBlockSums[i];
    *output = static_cast<int>(hostSum);

    delete[] hBlockSums;
    cudaFree(dBlockSums);
}