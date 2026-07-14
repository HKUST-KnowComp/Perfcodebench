#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int hist_s[256];
    // Initialize shared histogram to zero
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        hist_s[i] = 0;
    }
    __syncthreads();

    // Each thread processes a strided segment of the input
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    while (idx < n) {
        atomicAdd(&hist_s[input[idx]], 1U);
        idx += stride;
    }
    __syncthreads();

    // Merge shared histogram into global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], hist_s[i]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int kBlock = 256;
    const int grid = 1024;
    const size_t shared_mem_size = 256 * sizeof(unsigned int);
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock, shared_mem_size>>>(input, output, n);
    }
}