#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ partial_sums, int n) {
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

    // Shared memory reduction within block
    sdata[tid] = sum;
    __syncthreads();

    // Warp-level reduction using shuffle
    if (tid < 32) {
        for (int offset = 16; offset > 0; offset /= 2) {
            sum = __shfl_down_sync(0xffffffff, sum, offset);
        }
        // Only the first thread of each warp writes to shared memory
        if (tid == 0) {
            sdata[tid] = sum;
        }
    }
    __syncthreads();

    // Final block reduction in shared memory
    // Since we only have 32 values (one per warp) in sdata[0..31]
    if (tid < 32) {
        int val = sdata[tid];
        for (int offset = 16; offset > 0; offset /= 2) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (tid == 0) {
            partial_sums[blockIdx.x] = val;
        }
    }
}

__global__ void final_reduce_kernel(const int* __restrict__ partial_sums, int* __restrict__ output, int num_blocks) {
    int tid = threadIdx.x;
    int sum = 0;
    for (int i = tid; i < num_blocks; i += blockDim.x) {
        sum += partial_sums[i];
    }
    
    // Use atomicAdd for the very last step to handle small num_blocks
    if (tid == 0) {
        // We use a single block to finish the reduction to avoid complexity
        // but for safety in a general kernel, we use atomicAdd on the final output
        // However, since we launch this with 1 block, we can just write it.
    }
    
    // To keep it simple and fast, we'll use a single block for the final reduction
    // if num_blocks is small, or just use atomicAdd.
    atomicAdd(output, sum);
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        *output = 0;
        return;
    }

    const int kBlock = 256;
    // Calculate grid size to cover n, but cap it to avoid excessive overhead
    // We want enough blocks to saturate the GPU but not so many that partial_sums is huge
    const int max_blocks = 4096;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > max_blocks) grid = max_blocks;
    
    // Allocate temporary space for partial sums
    int* d_partial_sums;
    cudaMalloc(&d_partial_sums, max_blocks * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cudaMemset(d_partial_sums, 0, max_blocks * sizeof(int));

        // Step 1: Block-level reduction
        // We use a fixed grid size to ensure we don't exceed d_partial_sums
        // and to keep the workload balanced.
        int actual_grid = (grid > max_blocks) ? max_blocks : grid;
        
        reduce_kernel<<<actual_grid, kBlock, kBlock * sizeof(int)>>>(input, d_partial_sums, n);

        // Step 2: Final reduction of partial sums
        final_reduce_kernel<<<1, 256>>>(d_partial_sums, output, actual_grid);
    }

    cudaFree(d_partial_sums);
}