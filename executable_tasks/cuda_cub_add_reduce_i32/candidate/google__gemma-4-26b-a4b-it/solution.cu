#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int local_sum = 0;

    // Grid-stride loop to handle arbitrary n and ensure all elements are processed
    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    sdata[tid] = local_sum;
    __syncthreads();

    // In-block reduction using shuffle instructions
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Single atomic add per block to the global output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Configuration: Use a fixed number of blocks to balance occupancy and atomic contention
    // 256 threads per block is a good balance for most architectures
    const int threads_per_block = 256;
    const int num_blocks = 256; 
    const size_t shared_mem_size = threads_per_block * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to 0
        cudaMemset(output, 0, sizeof(int));
        
        // Launch optimized kernel
        optimized_reduce_kernel<num_blocks, threads_per_block><<<num_blocks, threads_per_block, shared_mem_size>>>(input, output, n);
    }
}