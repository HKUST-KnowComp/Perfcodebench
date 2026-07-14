#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use CUB BlockReduce for efficient intra-block reduction
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Storage storage;

    int thread_sum = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Grid-stride loop to handle n > total threads
    for (int i = idx; i < n; i += stride) {
        thread_sum += input[i];
    }

    // Reduce within the block
    int block_sum = BlockReduce(storage).Sum(thread_sum);

    // Only one thread per block adds the partial sum to the global output
    if (threadIdx.x == 0) {
        atomicAdd(output, block_sum);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int kBlock = 256;
    // Calculate grid size to maximize occupancy while avoiding excessive blocks
    // 1024 blocks is generally a good balance for most GPUs
    const int grid = (n + kBlock - 1) / kBlock;
    const int clamped_grid = (grid > 1024) ? 1024 : grid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<clamped_grid, kBlock>>>(input, output, n);
    }
}