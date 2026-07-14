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

    // Grid-stride loop to process all elements
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = tid; i < n; i += stride) {
        unsigned char val = input[i];
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
    // Using a block size of 256 is efficient for shared memory access patterns
    // and aligns well with the 256 bins.
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU without excessive overhead
    // A grid of 256-512 is usually sufficient for modern GPUs to hide latency.
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}