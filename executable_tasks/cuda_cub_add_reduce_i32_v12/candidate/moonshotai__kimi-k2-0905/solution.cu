#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
__global__ void block_reduce(const int* __restrict__ in,
                             int*          __restrict__ blockSums,
                             int                         n)
{
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int value = (tid < n) ? in[tid] : 0;
    int aggregate = BlockReduce(temp).Sum(value);

    if (threadIdx.x == 0) atomicAdd(blockSums, aggregate);
}
} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters)
{
    const int blockSize = 256;
    const int numBlocks = (n + blockSize - 1) / blockSize;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        block_reduce<<<numBlocks, blockSize>>>(input, output, n);
    }
}