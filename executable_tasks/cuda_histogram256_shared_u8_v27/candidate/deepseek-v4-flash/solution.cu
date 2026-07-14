#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int hist[256];

    // Initialize shared memory histogram to zero (all 256 bins)
    if (threadIdx.x < 256) {
        hist[threadIdx.x] = 0;
    }
    __syncthreads();

    // Grid-stride loop over input elements
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        atomicAdd(&hist[input[idx]], 1U);
    }
    __syncthreads();

    // Each thread writes one bin from shared to global memory atomically
    if (threadIdx.x < 256) {
        atomicAdd(output + threadIdx.x, hist[threadIdx.x]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = min(1024, (n + kBlock - 1) / kBlock);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}