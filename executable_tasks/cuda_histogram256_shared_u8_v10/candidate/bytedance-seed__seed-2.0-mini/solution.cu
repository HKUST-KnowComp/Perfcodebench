#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_atomic_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int block_hist[256];

    const int bin = threadIdx.x;
    // Initialize shared histogram to zero
    block_hist[bin] = 0;
    __syncthreads();

    // Process each input element for this thread
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    for (; idx < n; idx += blockDim.x * gridDim.x) {
        const unsigned char val = input[idx];
        atomicAdd(&block_hist[val], 1U);
    }
    __syncthreads();

    // Aggregate block's counts to global histogram
    atomicAdd(&output[bin], block_hist[bin]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = (n > 0) ? ((n + kBlock - 1) / kBlock) : 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output histogram to zero before each iteration
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        // Launch kernel only if there are valid input elements
        if (grid > 0) {
            histogram_shared_atomic_kernel<<<grid, kBlock>>>(input, output, n);
        }
    }
}