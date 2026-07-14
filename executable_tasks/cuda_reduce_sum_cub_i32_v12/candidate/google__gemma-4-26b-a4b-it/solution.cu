#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ partial_sums, int n) {
    extern __shared__ int sdata[];
    
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        sum += input[i];
        i += blockDim.x * gridDim.x;
    }

    sdata[tid] = sum;
    __syncthreads();

    // Block-level reduction using shuffle instructions
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Write block result to partial sums
    if (tid == 0) {
        partial_sums[blockIdx.x] = sdata[0];
    }
}

__global__ void final_reduce_kernel(const int* __restrict__ partial_sums, int* __restrict__ output, int num_blocks) {
    int sum = 0;
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < num_blocks; i += blockDim.x * gridDim.x) {
        sum += partial_sums[i];
    }

    // Use atomicAdd for the final small number of partial sums to ensure correctness
    // with minimal contention
    atomicAdd(output, sum);
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int threads_per_block = 256;
    // Calculate grid size to balance occupancy and partial sum buffer size
    const int grid_size = (n + threads_per_block - 1) / threads_per_block;
    const int optimized_grid = (grid_size > 1024) ? 1024 : grid_size;

    // Allocate temporary buffer for partial sums
    int* d_partial_sums;
    cudaMalloc(&d_partial_sums, optimized_grid * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cudaMemset(d_partial_sums, 0, optimized_grid * sizeof(int));

        // Stage 1: Block-level reduction
        reduce_kernel<<<optimized_grid, threads_per_block, threads_per_block * sizeof(int)>>>(
            input, d_partial_sums, n
        );

        // Stage 2: Final reduction of partial sums
        // We use a small grid for the final step
        final_reduce_kernel<<<1, 256>>>(d_partial_sums, output, optimized_grid);
    }

    cudaFree(d_partial_sums);
}