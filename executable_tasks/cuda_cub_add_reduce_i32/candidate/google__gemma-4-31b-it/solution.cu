#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use CUB BlockReduce for efficient intra-block reduction
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp_storage;

    int tid = threadIdx.x;
    int grid_size = gridDim.x * blockDim.x;
    int local_sum = 0;

    // Grid-stride loop to handle arbitrary n
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_size) {
        local_sum += input[i];
    }

    // Reduce local sums within the block
    int block_sum = BlockReduce(temp_storage).Sum(local_sum);

    // Only one thread per block performs the global atomic add
    if (tid == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Optimize block and grid size
    const int block_size = 256;
    // Limit grid size to avoid excessive atomic contention while saturating SMs
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 512) grid_size = 512;
    if (grid_size == 0) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_kernel<<<grid_size, block_size>>>(input, output, n);
    }
}