#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Each block maintains a local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements to cover the input
    // Using a grid-stride loop for robustness and efficiency
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        // Use atomicAdd on shared memory to handle multiple threads in the same block
        // hitting the same bin. Shared memory atomics are extremely fast.
        atomicAdd(&local_hist[val], 1U);
    }
    __syncthreads();

    // Aggregate the local histogram into the global output
    // We use a single thread per bin to perform the global atomicAdd
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is ideal for shared memory size (256 * 4 bytes = 1KB)
    constexpr int kBlock = 256;
    
    // Calculate grid size to ensure we have enough threads, but not too many to cause overhead
    // A grid size of 256-512 is usually a sweet spot for modern GPUs
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 512) grid = 512;
    if (grid < 1) grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output buffer
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        
        // Launch optimized kernel
        histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}