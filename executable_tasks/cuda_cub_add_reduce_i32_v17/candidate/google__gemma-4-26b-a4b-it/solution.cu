#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduction_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    // Using a power-of-two size for efficient indexing
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int sum = 0;
    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        sum += input[i];
        i += stride;
    }

    sdata[tid] = sum;
    __syncthreads();

    // In-block reduction using shared memory
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Single atomic add per block to the global output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Configuration: 256 threads per block is a good balance for occupancy and shared memory
    const int threads_per_block = 256;
    // Calculate grid size to maximize occupancy without exceeding hardware limits
    // We use a fixed number of blocks to ensure consistent performance across iterations
    const int blocks_per_grid = 256;
    const size_t shared_mem_size = threads_per_block * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to 0
        cudaMemset(output, 0, sizeof(int));
        
        // Launch reduction kernel
        reduction_kernel<<<blocks_per_grid, threads_per_block, shared_mem_size>>>(input, output, n);
    }
}