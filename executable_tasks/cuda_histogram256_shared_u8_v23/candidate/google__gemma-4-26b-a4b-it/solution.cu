#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Local histogram in shared memory
    // 256 bins * 4 bytes = 1024 bytes per block
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        local_hist[i] = 0;
    }
    __syncthreads();

    // Grid-stride loop to process input
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = tid; i < n; i += stride) {
        unsigned char val = input[i];
        // Use atomicAdd on shared memory to handle intra-block collisions
        atomicAdd(&local_hist[val], 1U);
    }
    __syncthreads();

    // Aggregate local histogram to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], local_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using 256 threads per block to match shared memory initialization and bin count
    constexpr int kBlock = 256;
    // A grid size that balances occupancy and shared memory usage
    // 1024 blocks * 256 threads = 262,144 threads per launch
    const int kGrid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset global histogram
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        
        // Launch optimized kernel
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}