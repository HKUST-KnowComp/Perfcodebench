#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdio>

namespace {

__global__ void block_reduce(const int* __restrict__ input,
                             int* __restrict__ blockSums,
                             int n)
{
    typedef cub::BlockReduce<int, 128> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int value = (tid < n) ? input[tid] : 0;
    int aggregate = BlockReduce(temp).Sum(value);

    if (threadIdx.x == 0) blockSums[blockIdx.x] = aggregate;
}

__global__ void final_sum(const int* __restrict__ blockSums,
                          int* __restrict__ output,
                          int numBlocks)
{
    int sum = 0;
    for (int i = threadIdx.x; i < numBlocks; i += blockDim.x)
        sum += blockSums[i];
    typedef cub::BlockReduce<int, 128> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;
    sum = BlockReduce(temp).Sum(sum);
    if (threadIdx.x == 0) *output = sum;
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block = 128;
    int numBlocks = 0;
    cudaOccupancyMaxActiveBlocksPerMultiprocessor(&numBlocks,
                                                   reinterpret_cast<const void*>(block_reduce),
                                                   block, 0);
    int smCount = 0;
    cudaDeviceGetAttribute(&smCount, cudaDevAttrMultiProcessorCount, 0);
    numBlocks *= smCount;

    int* dBlockSums = nullptr;
    cudaMalloc(&dBlockSums, sizeof(int) * numBlocks);

    for (int iter = 0; iter < iters; ++iter) {
        block_reduce<<<numBlocks, block>>>(input, dBlockSums, n);
        final_sum<<<1, block>>>(dBlockSums, output, numBlocks);
    }

    cudaFree(dBlockSums);
}