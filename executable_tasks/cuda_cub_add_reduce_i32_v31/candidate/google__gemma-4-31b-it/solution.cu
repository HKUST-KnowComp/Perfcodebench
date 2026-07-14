#include "interface.h"
#include <cuda_runtime.h>
#include <cooperative_groups.h>

namespace {

__global__ void reduce_i32_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int tid = threadIdx.x;
    int block_offset = blockIdx.x * blockDim.x;
    
    int local_sum = 0;
    // Grid-stride loop for coalesced memory access
    for (int i = block_offset + tid; i < n; i += gridDim.x * blockDim.x) {
        local_sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Shared memory for block-level reduction
    static __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int warp_id = tid / 32;
    if (tid % 32 == 0) {
        shared_sums[warp_id] = local_sum;
    }

    __syncthreads();

    // Final reduction of the 32 warp sums by the first warp
    if (warp_id < 32 && tid < 32) {
        int final_block_sum = shared_sums[tid];
        for (int offset = 16; offset > 0; offset /= 2) {
            final_block_sum += __shfl_down_sync(0xFFFFFFFF, final_block_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_block_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Use a reasonable number of blocks to saturate the GPU
    int threads_per_block = 256;
    int blocks_per_grid = (n + threads_per_block - 1) / threads_per_block;
    if (blocks_per_grid > 1024) blocks_per_grid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_i32_kernel<<<blocks_per_grid, threads_per_block>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}