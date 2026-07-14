#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>
#include <cub/block/block_reduce.cuh>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Use CUB BlockReduce for efficient intra-block reduction
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Arguments Arguments;

    int thread_id = threadIdx.x;
    int grid_stride = blockDim.x * gridDim.x;
    int local_sum = 0;

    // Grid-stride loop to handle arbitrary input sizes
    for (int idx = blockIdx.x * blockDim.x + thread_id; idx < n; idx += grid_stride) {
        local_sum += input[idx];
    }

    // Reduce the local sums within the block
    int block_sum = BlockReduce(Arguments).Sum(local_sum);

    // Only one thread per block performs the global atomic add
    if (thread_id == 0) {
        atomicAdd(output, block_sum);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int kBlockSize = 256;
    // Calculate grid size to saturate the GPU while avoiding excessive overhead
    // 1024 blocks is generally a good balance for most modern GPUs
    const int kGridSize = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<kGridSize, kBlockSize>>>(input, output, n);
    }
}