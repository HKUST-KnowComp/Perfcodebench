#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int sum = 0;
    // Grid-stride loop to handle n > total threads
    while (i < n) {
        sum += input[i];
        i += stride;
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

    // Single atomic add per block to the global output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized kernel for smaller blocks or when n is small
__global__ void reduce_kernel_small(const int* __restrict__ input, int* output, int n) {
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    int sum = 0;
    if (i < n) {
        sum = input[i];
    }

    extern __shared__ int sdata[];
    sdata[tid] = sum;
    __syncthreads();

    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine optimal block size and grid size
    // Using 256 threads per block is a good balance for occupancy and shared memory
    const int block_size = 256;
    // We want to saturate the GPU but not create too many blocks to avoid overhead
    // For very large n, we use a grid-stride approach.
    const int grid_size = (n + block_size - 1) / block_size;
    const int optimized_grid = (grid_size > 1024) ? 1024 : grid_size;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        
        // Use grid-stride kernel to handle arbitrary n efficiently
        // Shared memory size is block_size * sizeof(int)
        reduce_kernel<<<optimized_grid, block_size, block_size * sizeof(int)>>>(
            input, output, n
        );
    }
}