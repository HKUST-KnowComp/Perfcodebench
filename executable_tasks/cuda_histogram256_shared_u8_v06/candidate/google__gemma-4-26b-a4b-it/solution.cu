#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_optimized_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Each block has its own local histogram in shared memory
    // 256 bins * 4 bytes = 1024 bytes per block
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Each thread processes elements using a stride to cover the input
    // We use a local loop to maximize occupancy and reuse
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = tid; i < n; i += stride) {
        unsigned char val = input[i];
        // Use atomicAdd on shared memory to handle intra-block collisions
        // Shared memory atomics are extremely fast on modern architectures
        atomicAdd(&s_hist[val], 1U);
    }
    __syncthreads();

    // Merge local shared histogram into global output
    // Each thread handles one bin to minimize global atomic contention
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using 256 threads per block to match the bin count for efficient shared memory distribution
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU. 1024 blocks is a good starting point for most GPUs.
    const int kGrid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset global histogram
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        
        // Launch kernel
        histogram_optimized_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}