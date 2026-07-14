#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Each block maintains its own local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements using a grid-stride loop
    // To maximize shared memory usage, we use a local register-based cache if needed,
    // but for 256 bins, direct shared memory access is efficient enough.
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        unsigned char val = input[idx];
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
    // Using a block size of 256 to match the number of bins for easy aggregation
    constexpr int kBlock = 256;
    // Grid size can be tuned; 1024 blocks is generally sufficient for modern GPUs
    const int kGrid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output buffer
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        
        // Launch the optimized kernel
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}