#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle arbitrary n
    // Each thread accumulates its own local sum first to reduce shared memory pressure
    while (i < n) {
        sum += input[i];
        i += blockDim.x * gridDim.x;
    }

    sdata[tid] = sum;
    __syncthreads();

    // Warp-level reduction using shuffle instructions
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    // Only the first thread of the block writes to the global output via atomicAdd
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized kernel using warp shuffles for even faster intra-warp reduction
__global__ void reduce_sum_warp_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (i < n) {
        sum += input[i];
        i += stride;
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Shared memory to communicate between warps in a block
    __shared__ int warp_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        warp_sums[wid] = sum;
    }

    __syncthreads();

    // Final reduction within the block
    if (wid == 0) {
        int block_sum = (threadIdx.x < (blockDim.x / 32)) ? warp_sums[lane] : 0;
        // Final warp reduction for the block
        for (int offset = 16; offset > 0; offset >>= 1) {
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
    // A grid size of 256-512 is usually a sweet spot for modern GPUs
    const int grid = (n + kBlock - 1) / kBlock;
    const int optimized_grid = grid > 256 ? 256 : grid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the warp-shuffle optimized kernel
        reduce_sum_warp_kernel<<<optimized_grid, kBlock>>>(input, output, n);
    }
}