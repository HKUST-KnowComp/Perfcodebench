#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cub>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use a shared memory array for block-level reduction
    extern __shared__ int shared_mem[];
    
    int tid = threadIdx.x;
    int block_offset = blockIdx.x * blockDim.x;
    int local_sum = 0;

    // Grid-stride loop to process all elements
    for (int i = block_offset + tid; i < n; i += gridDim.x * blockDim.x) {
        local_sum += input[i];
    }

    // Block-level reduction using a simple tree
    shared_mem[tid] = local_sum;
    __syncthreads();

    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            shared_mem[tid] += shared_mem[tid + s];
        }
        __syncthreads();
    }

    // Only the first thread of the block performs the global atomic add
    if (tid == 0) {
        atomicAdd(output, shared_mem[0]);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to saturate the GPU (approx 100-200 blocks per SM)
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 1024) grid_size = 1024;
    if (grid_size == 0) grid_size = 1;

    size_t shared_mem_size = block_size * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_kernel<<<grid_size, block_size, shared_mem_size>>>(input, output, n);
    }
}