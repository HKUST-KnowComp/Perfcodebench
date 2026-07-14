#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int smem_hist[256];

    // Initialize shared histogram to zero using all threads
    smem_hist[threadIdx.x] = 0;
    __syncthreads();

    // Process bytes in grid-stride loop
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    while (idx < n) {
        unsigned char val = input[idx];
        atomicAdd(&smem_hist[val], 1U);
        idx += blockDim.x * gridDim.x;
    }

    __syncthreads();

    // Reduce block histogram to global memory using atomics
    atomicAdd(&output[threadIdx.x], smem_hist[threadIdx.x]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    if (grid == 0) grid = 1;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
