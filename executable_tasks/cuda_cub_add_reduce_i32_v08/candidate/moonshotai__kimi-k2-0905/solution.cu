#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdio>

namespace {

constexpr int BLOCK_THREADS = 256;

__global__ void blockReduceKernel(const int* __restrict__ input,
                                  int* __restrict__ blockSums,
                                  int n)
{
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    // Load and first-level reduction in shared memory
    __shared__ int smem[BLOCK_THREADS];
    smem[tid] = (idx < n) ? input[idx] : 0;
    __syncthreads();

    // Unroll last 5 warps
    #pragma unroll
    for (int offset = BLOCK_THREADS / 2; offset >= 32; offset >>= 1) {
        if (tid < offset) smem[tid] += smem[tid + offset];
        __syncthreads();
    }

    // Warp shuffle reduction
    int val = smem[tid];
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1)
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);

    // Write block aggregate
    if (tid == 0) blockSums[blockIdx.x] = val;
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters)
{
    // Temporary storage for CUB final reduction
    static int* d_blockSums = nullptr;
    static void* d_tempStorage = nullptr;
    static size_t tempStorageBytes = 0;
    static int maxBlocks = 0;

    // One-time allocation
    if (d_blockSums == nullptr) {
        int blocks = (n + BLOCK_THREADS - 1) / BLOCK_THREADS;
        cudaMalloc(&d_blockSums, sizeof(int) * blocks);
        maxBlocks = blocks;
        cub::DeviceReduce::Sum(nullptr, tempStorageBytes,
                               d_blockSums, output, maxBlocks);
        cudaMalloc(&d_tempStorage, tempStorageBytes);
    }

    for (int iter = 0; iter < iters; ++iter) {
        int blocks = (n + BLOCK_THREADS - 1) / BLOCK_THREADS;
        blockReduceKernel<<<blocks, BLOCK_THREADS>>>(input, d_blockSums, n);
        cub::DeviceReduce::Sum(d_tempStorage, tempStorageBytes,
                               d_blockSums, output, blocks);
    }
}