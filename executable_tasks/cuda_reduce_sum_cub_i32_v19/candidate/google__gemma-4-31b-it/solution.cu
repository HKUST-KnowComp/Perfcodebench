#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Use CUB BlockReduce to sum elements within a block
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Shared_Storage shared_storage;
    BlockReduce block_reduce(shared_storage);

    int thread_sum = 0;
    int tid = threadIdx.x;
    int grid_stride = blockDim.x * gridDim.x;

    // Grid-stride loop for coalesced memory access
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_stride) {
        thread_sum += input[i];
    }

    // Reduce within the block
    int block_sum = block_reduce.Sum(thread_sum);

    // Only one thread per block adds the result to the global output
    if (block_reduce.CUB_BLOCK_OFFSET == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to saturate GPU occupancy
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 1024) grid_size = 1024; // Cap grid size to prevent excessive atomic contention
    if (grid_size == 0 && n > 0) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        if (n > 0) {
            reduce_sum_kernel<<<grid_size, block_size>>>(input, output, n);
        }
    }
}