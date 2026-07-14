#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_local_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];

    // Initialize shared histogram to zero using all threads in the block
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Grid-stride loop to build per-block histogram in shared memory
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += gridDim.x * blockDim.x) {
        unsigned char val = input[idx];
        atomicAdd(&s_hist[val], 1U);
    }

    __syncthreads();

    // Atomically reduce per-block histogram to global output, skipping zero bins
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        unsigned int count = s_hist[i];
        if (count > 0) {
            atomicAdd(&output[i], count);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_local_atomic_kernel<<<grid, kBlock>>>(input, output, n);
    }
}