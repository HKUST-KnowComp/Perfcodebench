#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for block-level reduction
    // Using a size that accommodates the max block size (typically 1024)
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        sum += input[i];
        i += blockDim.x * gridDim.x;
    }

    sdata[tid] = sum;
    __syncthreads();

    // Warp-level reduction using shuffle instructions
    if (tid < blockDim.x) {
        for (int offset = blockDim.x / 2; offset > 0; offset /= 2) {
            if (tid < offset) {
                sdata[tid] += sdata[tid + offset];
            }
            __syncthreads();
        }
        
        // Only the first thread in the block writes to global memory via atomicAdd
        if (tid == 0) {
            atomicAdd(output, sdata[0]);
        }
    }
}

// A more robust version using warp shuffles for better performance
__global__ void reduce_sum_optimized_kernel(const int* __restrict__ input, int* output, int n) {
    int sum = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Grid-stride loop
    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }

    // Warp reduction using shuffle
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Shared memory to aggregate warp results within a block
    __shared__ int warp_sums[32]; // Max warps in a block (1024/32)
    int lane = threadIdx.x % warpSize;
    int wid = threadIdx.x / warpSize;

    if (lane == 0) {
        warp_sums[wid] = sum;
    }

    __syncthreads();

    // Final reduction within the block
    // Only the first warp handles the reduction of warp_sums
    if (wid == 0) {
        int block_sum = (threadIdx.x < (blockDim.x / warpSize)) ? warp_sums[lane] : 0;
        
        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
            block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
        }

        if (lane == 0) {
            atomicAdd(output, block_sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int kBlock = 256;
    // Calculate grid size to ensure we cover n, but cap it to avoid excessive overhead
    // A common heuristic is to use a reasonable number of blocks
    const int grid = (n + kBlock - 1) / kBlock;
    const int optimized_grid = grid > 1024 ? 1024 : grid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the optimized kernel with warp shuffles
        reduce_sum_optimized_kernel<<<optimized_grid, kBlock>>>(input, output, n);
    }
}