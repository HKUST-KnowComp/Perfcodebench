#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input, unsigned int* output, int n) {
    // Local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes elements using a grid-stride loop
    // To maximize throughput, we use the thread index to map to the input
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Reduce local histogram to global memory
    // We use a simple approach where each thread handles one bin
    // or we can have threads cooperate to reduce the 256 bins.
    // For 256 bins and 256 threads, it's a 1:1 mapping.
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

// Alternative approach for higher performance: 
// Use a larger block size or multiple passes if N is massive, 
// but for standard histogram tasks, shared memory atomics are the bottleneck breaker.

__global__ void histogram256_optimized_kernel(const unsigned char* __restrict__ input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Process input
    for (int i = idx; i < n; i += stride) {
        atomicAdd(&s_hist[input[i]], 1U);
    }
    __syncthreads();

    // Write back to global memory
    // To avoid massive contention on global memory, we use the thread to distribute the 256 bins
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using a block size of 256 is optimal for the shared memory size of 256 * 4 bytes = 1KB
    // This allows high occupancy.
    const int kBlock = 256;
    // Calculate grid size to ensure we cover the data but don't oversubscribe too much
    // A grid size of 256-512 is usually a sweet spot for modern GPUs
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_optimized_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}