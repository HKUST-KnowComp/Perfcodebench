#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Shared_Storage shared_storage;

    int thread_id = threadIdx.x;
    int grid_offset = blockIdx.x * blockDim.x;

    int local_sum = 0;
    // Grid-stride loop to handle n > grid * blockDim
    for (int i = grid_offset + thread_id; i < n; i += gridDim.x * blockDim.x) {
        local_sum += input[i];
    }

    // Reduce within the block
    int block_sum = BlockReduce(shared_storage).Sum(local_sum);

    // Only one thread per block performs the global atomic add
    if (thread_id == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to saturate GPU while avoiding excessive overhead
    // 1024 blocks is generally a good balance for most architectures
    const int grid_size = (n + block_size - 1) / block_size;
    const int clamped_grid_size = (grid_size < 1024) ? grid_size : 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<clamped_grid_size, block_size>>>(input, output, n);
    }
}