#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.hpp>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use CUB BlockReduce to sum elements within a block
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Storage storage;

    int thread_id = threadIdx.x;
    int grid_dim = gridDim.x * blockDim.x;
    int sum = 0;

    // Grid-stride loop to process all elements
    for (int i = thread_id + blockIdx.x * blockDim.x; i < n; i += grid_dim) {
        sum += input[i];
    }

    // Reduce the local sums within the block
    int block_sum = BlockReduce(storage).Sum(sum);

    // Only the first thread of the block adds the result to the global output
    if (thread_id == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to maximize occupancy while avoiding excessive overhead
    // 1024 blocks is generally a good balance for most GPUs
    const int grid_size = (n + block_size - 1) / block_size;
    const int clamped_grid_size = (grid_size < 1024) ? grid_size : 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<clamped_grid_size, block_size>>>(input, output, n);
    }
}