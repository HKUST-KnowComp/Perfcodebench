#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Each block maintains a local histogram in shared memory
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
        atomicAdd(&local_hist[val], 1U);
    }
    __syncthreads();

    // Aggregate local histogram into global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], local_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using 256 threads per block to match shared memory initialization pattern
    // and 128 blocks to balance occupancy and shared memory usage.
    constexpr int kBlock = 256;
    constexpr int kGrid = 128;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}