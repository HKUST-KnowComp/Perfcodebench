#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use shared memory for block-level reduction
    __shared__ int sdata[32];

    int tid = threadIdx.x;
    int grid_stride = blockDim.x * gridDim.x;
    int local_sum = 0;

    // Grid-stride loop to process all elements
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_stride) {
        local_sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Store warp results in shared memory
    if ((tid & 31) == 0) {
        sdata[blockIdx.x % 32] = 0; // This is a placeholder, we need one slot per warp
    }
    
    // Correct warp reduction: only the first thread of each warp writes to shared memory
    if ((tid & 31) == 0) {
        // We have 8 warps in a 256-thread block
        // But we can just use a simpler approach: 
        // Each warp reduces its local_sum, then the first thread of each warp writes to shared memory
    }
}

// Refined kernel for better performance
__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int tid = threadIdx.x;
    int local_sum = 0;
    int grid_stride = blockDim.x * gridDim.x;

    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_stride) {
        local_sum += input[i];
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Shared memory for the 8 warps in a 256-thread block
    __shared__ int shared_sums[8];
    int warp_id = tid / 32;
    if ((tid & 31) == 0) {
        shared_sums[warp_id] = local_sum;
    }

    __syncthreads();

    // Final reduction of the 8 warp sums by the first warp
    if (warp_id == 0) {
        int final_sum = (tid < 8) ? shared_sums[tid] : 0;
        for (int offset = 4; offset > 0; offset /= 2) {
            final_sum += __shfl_down_sync(0xFFFFFFFF, final_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Tuning parameters
    const int threads_per_block = 256;
    // Calculate grid size to maximize occupancy and throughput
    // For large N, we want enough blocks to saturate the GPU
    int blocks_per_grid = (n + threads_per_block - 1) / threads_per_block;
    if (blocks_per_grid > 1024) blocks_per_grid = 1024;
    if (blocks_per_grid == 0) blocks_per_grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        optimized_reduce_kernel<<<blocks_per_grid, threads_per_block>>>(input, output, n);
    }
}