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

    // Each thread processes multiple elements to cover the input size
    // Using a grid-stride loop for robustness and performance
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Reduce local histogram to global memory
    // We use a simple approach where each thread handles one bin
    // to minimize global atomic contention.
    for (int i = tid; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], local_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // 256 threads per block allows 1:1 mapping for the 256 bins during reduction
    constexpr int kBlock = 256;
    // Calculate grid size to ensure we have enough threads to cover N, 
    // but keep it reasonable to avoid excessive shared memory overhead.
    // A grid of 256-512 is usually a sweet spot for modern GPUs.
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}