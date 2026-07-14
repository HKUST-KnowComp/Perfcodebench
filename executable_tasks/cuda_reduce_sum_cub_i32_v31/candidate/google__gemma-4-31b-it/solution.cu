#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Shared_Storage shared_storage;

    int thread_id = threadIdx.x;
    int offset = blockIdx.x * blockDim.x + thread_id;

    int local_sum = 0;
    if (offset < n) {
        // Grid-stride loop to handle n > grid * blockDim
        for (int i = offset; i < n; i += gridDim.x * blockDim.x) {
            local_sum += input[i];
        }
    }

    // Reduce within the block
    int block_sum = BlockReduce(shared_storage).Sum(local_sum);

    // Only one thread per block adds the partial sum to the global output
    if (thread_id == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to maximize occupancy and cover the data
    // Using a fixed large grid or calculating based on n
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 1024) grid_size = 1024; 
    if (grid_size == 0 && n > 0) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        if (n > 0) {
            reduce_sum_kernel<<<grid_size, block_size>>>(input, output, n);
        }
    }
}