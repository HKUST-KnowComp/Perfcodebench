#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int local_sum = 0;

    // Grid-stride loop to handle arbitrary n and ensure all elements are processed
    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    sdata[tid] = local_sum;
    __syncthreads();

    // In-block reduction using warp shuffles
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Only the first thread in the block writes to global memory via atomicAdd
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine optimal block and grid dimensions
    // 256 threads per block is a good balance for occupancy and shuffle efficiency
    const int threads_per_block = 256;
    // Limit grid size to prevent excessive atomic contention on the single output address
    const int blocks_per_grid = (n + threads_per_block - 1) / threads_per_block;
    const int optimized_grid = (blocks_per_grid > 1024) ? 1024 : blocks_per_grid;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output
        cudaMemset(output, 0, sizeof(int));
        
        // Launch kernel with shared memory size
        optimized_reduce_kernel<<<optimized_grid, threads_per_block, threads_per_block * sizeof(int)>>>(
            input, 
            output, 
            n
        );
    }
}