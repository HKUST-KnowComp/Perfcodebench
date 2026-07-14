#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_sum_kernel(const int* input, int* block_sums, int n) {
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Shared_Storage shared_storage;

    int tid = threadIdx.x;
    int offset = blockIdx.x * blockDim.x + tid;

    int local_sum = 0;
    if (offset < n) {
        // Grid-stride loop to handle n > grid * blockDim
        for (int i = offset; i < n; i += blockDim.x * gridDim.x) {
            local_sum += input[i];
        }
    }

    int block_sum = BlockReduce(shared_storage).Sum(local_sum);

    if (tid == 0) {
        block_sums[blockIdx.x] = block_sum;
    }
}

__global__ void final_reduce_kernel(const int* block_sums, int* output, int num_blocks) {
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Shared_Storage shared_storage;

    int tid = threadIdx.x;
    int local_sum = 0;
    if (tid < num_blocks) {
        local_sum = block_sums[tid];
    }

    int total_sum = BlockReduce(shared_storage).Sum(local_sum);

    if (tid == 0) {
        *output = total_sum;
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        cudaMemset(output, 0, sizeof(int));
        return;
    }

    const int block_size = 256;
    const int grid_size = (n + block_size - 1) / block_size;
    // Cap grid size to avoid excessive memory for block_sums
    const int limited_grid = (grid_size > 1024) ? 1024 : grid_size;

    int* d_block_sums;
    cudaMalloc(&d_block_sums, limited_grid * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_sum_kernel<<<limited_grid, block_size>>>(input, d_block_sums, n);
        final_reduce_kernel<<<1, block_size>>>(d_block_sums, output, limited_grid);
    }

    cudaFree(d_block_sums);
}