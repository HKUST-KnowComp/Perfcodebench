#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warpReduceSum(int val) {
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int val = (i < n) ? input[i] : 0;

    // Warp reduction within shared memory
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }

    // Store warp results in shared memory
    if ((tid % 32) == 0) {
        sdata[tid / 32] = val;
    }

    __syncthreads();

    // Final reduction in shared memory for the block
    // Only the first warp of the block participates in the final shared memory reduction
    if (tid < blockDim.x / 32) {
        int lane = tid % 32;
        int warp_val = sdata[tid];
        for (int offset = 16; offset > 0; offset /= 2) {
            warp_val += __shfl_down_sync(0xffffffff, warp_val, offset);
        }
        if (lane == 0) {
            atomicAdd(output, warp_val);
        }
    }
}

// A more robust version using a single shared memory array for all warps
__global__ void reduce_sum_kernel_v2(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[32]; // Max 1024 threads / 32 = 32 warps
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    int val = (i < n) ? input[i] : 0;

    // Warp shuffle reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }

    // Write warp result to shared memory
    int lane = tid % 32;
    int warp_id = tid / 32;
    if (lane == 0) {
        sdata[warp_id] = val;
    }

    __syncthreads();

    // Final reduction of the warp results in shared memory
    if (warp_id == 0) {
        int final_val = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
        // Re-use shuffle for the shared memory values
        for (int offset = 16; offset > 0; offset /= 2) {
            final_val += __shfl_down_sync(0xffffffff, final_val, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_val);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int threadsPerBlock = 256;
    const int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    // Cap blocks to prevent excessive atomic contention if n is massive
    const int optimizedBlocks = (blocksPerGrid > 4096) ? 4096 : blocksPerGrid;
    
    // We use a fixed grid size for the kernel to ensure predictable performance
    // but we must ensure we cover all elements. 
    // To keep it simple and correct, we use a standard grid.
    const int grid = (n + threadsPerBlock - 1) / threadsPerBlock;
    // However, to avoid too many atomics, we cap the grid.
    // If grid is too large, we'd need a multi-pass approach. 
    // For this task, a single pass with a reasonable grid is usually enough.
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the v2 kernel with shared memory for warp results
        // Shared memory size: 32 * sizeof(int) = 128 bytes
        reduce_sum_kernel_v2<<<grid, threadsPerBlock, 128>>>(input, output, n);
    }
}