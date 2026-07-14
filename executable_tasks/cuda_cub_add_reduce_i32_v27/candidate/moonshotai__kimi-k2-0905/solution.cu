#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
__global__ void cub_block_reduce(const int* __restrict__ input,
                                 int*               output,
                                 int                n,
                                 int*               blockSums)
{
    // Specialise CUB block reduce for int
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int value = 0;
    if (tid < n) value = input[tid];

    int blockSum = BlockReduce(temp).Sum(value);

    if (threadIdx.x == 0) atomicAdd(output, blockSum);
}
} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters)
{
    const int blockSize = 256;
    const int gridSize  = (n + blockSize - 1) / blockSize;

    for (int i = 0; i < iters; ++i)
    {
        cudaMemset(output, 0, sizeof(int));
        cub_block_reduce<<<gridSize, blockSize>>>(input, output, n, nullptr);
    }
}