#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void optimized_histogram(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int sh[256];

    // Initialize shared histogram bins to zero
    sh[threadIdx.x] = 0;
    __syncthreads();

    // Grid-stride loop to process all input elements
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        const unsigned char val = input[idx];
        atomicAdd(&sh[val], 1U);
    }

    // Wait for all threads to finish updating shared histogram
    __syncthreads();

    // Aggregate shared histogram to global output
    atomicAdd(output + threadIdx.x, sh[threadIdx.x]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlockSize = 256;
    constexpr int kGridSize = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        optimized_histogram<<<kGridSize, kBlockSize>>>(input, output, n);
    }
}