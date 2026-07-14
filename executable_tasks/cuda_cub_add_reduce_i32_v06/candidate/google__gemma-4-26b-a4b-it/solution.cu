#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle large n and ensure all elements are processed
    // Each thread accumulates its own local sum first to reduce shared memory pressure
    int stride = blockDim.x * gridDim.x;
    for (; i < n; i += stride) {
        sum += input[i];
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

    // Single thread per block writes to global memory via atomicAdd
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Configuration: Use a reasonable number of blocks to saturate the GPU
    // but not so many that we cause excessive overhead.
    const int threads_per_block = 256;
    const int blocks_per_grid = 256; 
    const size_t shared_mem_size = threads_per_block * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output
        cudaMemset(output, 0, sizeof(int));

        // Launch reduction kernel
        // We use a grid-stride approach to ensure correctness regardless of n
        reduce_kernel<<<blocks_per_grid, threads_per_block, shared_mem_size>>>(input, output, n);
    }
}