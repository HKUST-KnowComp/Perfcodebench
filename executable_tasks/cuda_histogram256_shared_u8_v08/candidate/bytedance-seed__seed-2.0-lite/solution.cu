#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_optimized_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Per-block private histogram in low-latency shared memory
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared histogram to zero
    if (threadIdx.x < 256) {
        s_hist[threadIdx.x] = 0U;
    }
    __syncthreads();

    const int total_threads = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    const int n4 = n / 4;

    // Process input in 4-byte chunks for maximum memory throughput
    for (; idx < n4; idx += total_threads) {
        const unsigned int chunk = reinterpret_cast<const unsigned int*>(input)[idx];
        // Extract all 4 bytes from the 32-bit load
        const unsigned char b0 = chunk & 0xFF;
        const unsigned char b1 = (chunk >> 8) & 0xFF;
        const unsigned char b2 = (chunk >> 16) & 0xFF;
        const unsigned char b3 = (chunk >> 24) & 0xFF;
        // Update shared histogram with fast on-chip atomics
        atomicAdd(&s_hist[b0], 1U);
        atomicAdd(&s_hist[b1], 1U);
        atomicAdd(&s_hist[b2], 1U);
        atomicAdd(&s_hist[b3], 1U);
    }

    // Process remaining 0-3 input bytes to cover full input range
    const int remainder_start = n4 * 4;
    idx = remainder_start + threadIdx.x;
    for (; idx < n; idx += blockDim.x) {
        const unsigned char b = input[idx];
        atomicAdd(&s_hist[b], 1U);
    }

    // Wait for all block threads to finish accumulation before merging to global memory
    __syncthreads();

    // Merge block's local histogram into global output with minimal global atomics
    if (threadIdx.x < 256) {
        atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_optimized_kernel<<<grid, kBlock>>>(input, output, n);
  }
}