#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int local_sum = 0;
    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    sdata[tid] = local_sum;
    __syncthreads();

    // Block-level reduction using shuffle instructions
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Single atomic add per block to global memory
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Fallback for small block sizes or non-power-of-two if necessary,
// but for performance we assume standard power-of-two block sizes.
// A more robust version would use a warp-shuffle reduction.
__global__ void warp_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int local_sum = 0;
    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    // Warp reduction using __shfl_down_sync
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Shared memory to communicate between warps in a block
    __shared__ int shared_warp_sums[32]; // Max warps in a block (assuming 1024 threads)
    int lane = tid % warpSize;
    int wid = tid / warpSize;

    if (lane == 0) {
        shared_warp_sums[wid] = local_sum;
    }
    __syncthreads();

    // Final reduction within the block
    if (wid == 0) {
        int block_sum = (tid < (blockDim.x / warpSize)) ? shared_warp_sums[tid] : 0;
        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
            block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, block_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine optimal grid size
    // We want to saturate the GPU but keep enough blocks to hide latency
    int blockSize = 256;
    int gridSize = (n + blockSize - 1) / blockSize;
    if (gridSize > 1024) gridSize = 1024;
    if (gridSize < 1) gridSize = 1;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output
        *output = 0;
        
        // Launch kernel with warp-shuffle optimization
        // Using 256 threads per block is a good balance for most architectures
        warp_reduce_kernel<<<gridSize, blockSize>>>(input, output, n);
    }
}