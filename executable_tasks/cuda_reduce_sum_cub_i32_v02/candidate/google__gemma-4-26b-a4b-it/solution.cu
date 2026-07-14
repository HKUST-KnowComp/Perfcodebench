#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for block-level reduction
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

    // Perform reduction in shared memory using warp shuffles
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Only the first thread in the block writes to global memory via atomicAdd
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Tuning parameters
    // Using a power-of-two block size for efficient reduction
    const int kBlock = 256;
    // Calculate grid size to ensure we cover n, but cap it to avoid excessive overhead
    // A grid size of 256-512 is usually a sweet spot for modern GPUs
    const int kGrid = 256;

    // Calculate shared memory size needed
    size_t shared_mem_size = kBlock * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to 0
        cudaMemset(output, 0, sizeof(int));
        
        // Launch kernel
        reduce_sum_kernel<<<kGrid, kBlock, shared_mem_size>>>(input, output, n);
    }
}