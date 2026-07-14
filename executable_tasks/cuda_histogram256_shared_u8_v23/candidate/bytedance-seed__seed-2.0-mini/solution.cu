#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int smem[256];

    // Initialize shared memory bins to zero
    const unsigned int bin = threadIdx.x;
    smem[bin] = 0;
    __syncthreads();

    // Process input elements with strided indexing
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    while (idx < n) {
        const unsigned char val = input[idx];
        atomicAdd(&smem[val], 1U);
        idx += blockDim.x * gridDim.x;
    }

    __syncthreads();

    // Aggregate partial block sums to global memory
    atomicAdd(&output[bin], smem[bin]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}