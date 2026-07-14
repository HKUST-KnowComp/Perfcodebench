#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        local_hist[i] = 0;
    }
    __syncthreads();

    // Each thread processes elements using a grid-stride loop
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&local_hist[val], 1U);
    }
    __syncthreads();

    // Aggregate local histogram to global memory
    // We use a single thread per bin to perform the global atomic add
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], local_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using a block size of 256 is efficient for shared memory and occupancy
    constexpr int kBlock = 256;
    // Calculate grid size to ensure we cover the input or use a fixed large grid
    // For performance, we use a fixed grid size to avoid launch overhead in loops
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}