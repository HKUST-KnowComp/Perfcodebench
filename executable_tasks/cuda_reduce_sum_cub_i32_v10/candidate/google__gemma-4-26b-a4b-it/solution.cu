#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ partial_sums, int n) {
    __shared__ int sdata[256];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle large n
    while (i < n) {
        sum += input[i];
        i += blockDim.x * gridDim.x;
    }

    sdata[tid] = sum;
    __syncthreads();

    // Warp-level reduction using shuffle
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        if (tid < warpSize) {
            sdata[tid] += __shfl_down_sync(0xFFFFFFFF, sdata[tid], offset);
        }
        __syncthreads();
    }

    // Write block result to partial_sums
    if (tid == 0) {
        partial_sums[blockIdx.x] = sdata[0];
    }
}

__global__ void final_reduce_kernel(const int* partial_sums, int* output, int num_blocks) {
    int tid = threadIdx.x;
    int sum = 0;
    for (int i = tid; i < num_blocks; i += blockDim.x) {
        sum += partial_sums[i];
    }

    // Final warp reduction for the single block
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    if (tid == 0) {
        atomicAdd(output, sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int threads_per_block = 256;
    const int blocks_per_grid = 256; // Sufficient for most workloads
    
    // Allocate temporary buffer for partial sums
    int* d_partial_sums;
    cudaMalloc(&d_partial_sums, blocks_per_grid * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cudaMemset(d_partial_sums, 0, blocks_per_grid * sizeof(int));

        // Step 1: Block-level reduction
        reduce_kernel<<<blocks_per_grid, threads_per_block>>>(input, d_partial_sums, n);

        // Step 2: Final aggregation
        final_reduce_kernel<<<1, threads_per_block>>>(d_partial_sums, output, blocks_per_grid);
    }

    cudaFree(d_partial_sums);
}