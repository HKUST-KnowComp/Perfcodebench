#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use CUB BlockReduce to sum elements within a block
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Shared_Storage shared_storage;
    BlockReduce block_reduce(shared_storage);

    int thread_sum = 0;
    int tid = threadIdx.x;
    int grid_stride = blockDim.x * gridDim.x;

    // Grid-stride loop for coalesced access
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_stride) {
        thread_sum += input[i];
    }

    // Reduce within the block
    int block_sum = block_reduce.Sum(thread_sum);

    // Only one thread per block adds the result to the global output
    if (block_reduce.ThreadId() == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to saturate the GPU without excessive overhead
    // 1024 blocks is generally a good balance for most architectures
    const int grid_size = (n + block_size - 1) / block_size;
    const int clamped_grid_size = (grid_size < 1024) ? grid_size : 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<clamped_grid_size, block_size>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}