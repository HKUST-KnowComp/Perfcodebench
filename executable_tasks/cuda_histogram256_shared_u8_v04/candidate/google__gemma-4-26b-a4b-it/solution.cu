#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements to cover the input
    // Using a grid-stride loop for robustness and performance
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Reduce local histogram to global memory
    // We use a single thread per bin to perform the final atomicAdd to global memory
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

// Alternative kernel for very large N to ensure better coalescing and occupancy
__global__ void histogram256_optimized_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Process data
    for (int i = idx; i < n; i += stride) {
        atomicAdd(&s_hist[input[i]], 1U);
    }
    __syncthreads();

    // Final reduction
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Tuning parameters
    const int threadsPerBlock = 256;
    // Calculate grid size to ensure we cover N, but cap it to avoid excessive overhead
    // A grid size of ~256-512 blocks is usually a sweet spot for modern GPUs
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    if (blocksPerGrid > 512) blocksPerGrid = 512;
    if (blocksPerGrid < 1) blocksPerGrid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_optimized_kernel<<<blocksPerGrid, threadsPerBlock>>>(input, output, n);
    }
}
