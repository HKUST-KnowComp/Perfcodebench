#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
constexpr int BLOCK_THREADS = 256;
constexpr int ITEMS_PER_THREAD = 4;
constexpr int TILE_SIZE = BLOCK_THREADS * ITEMS_PER_THREAD;

__global__ void tile_reduce_kernel(const int* input, int* block_sums, int n) {
    int tile_idx = blockIdx.x;
    int tile_offset = tile_idx * TILE_SIZE;
    int* tile_ptr = const_cast<int*>(input) + tile_offset;
    int valid_items = (tile_offset + TILE_SIZE <= n) ? TILE_SIZE : (n - tile_offset);

    int thread_data[ITEMS_PER_THREAD];
    cub::LoadDirectStriped<BLOCK_THREADS>(threadIdx.x, tile_ptr, thread_data, valid_items, 0);

    int thread_sum = 0;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) thread_sum += thread_data[i];

    typedef cub::BlockReduce<int, BLOCK_THREADS> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp_storage;
    int block_sum = BlockReduce(temp_storage).Sum(thread_sum);

    if (threadIdx.x == 0) atomicAdd(block_sums + tile_idx, block_sum);
}

__global__ void final_reduce_kernel(const int* block_sums, int* output, int num_blocks) {
    typedef cub::BlockReduce<int, BLOCK_THREADS> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp_storage;
    int thread_sum = 0;
    for (int i = threadIdx.x; i < num_blocks; i += blockDim.x) thread_sum += block_sums[i];
    int block_sum = BlockReduce(temp_storage).Sum(thread_sum);
    if (threadIdx.x == 0) atomicAdd(output, block_sum);
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int* d_out;
    cudaMalloc(&d_out, sizeof(int));
    int num_blocks = (n + TILE_SIZE - 1) / TILE_SIZE;
    int* d_block_sums;
    cudaMalloc(&d_block_sums, num_blocks * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(d_out, 0, sizeof(int));
        cudaMemsetAsync(d_block_sums, 0, num_blocks * sizeof(int));
        tile_reduce_kernel<<<num_blocks, BLOCK_THREADS>>>(input, d_block_sums, n);
        final_reduce_kernel<<<1, BLOCK_THREADS>>>(d_block_sums, d_out, num_blocks);
    }
    cudaMemcpyAsync(output, d_out, sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(d_block_sums);
    cudaFree(d_out);
}