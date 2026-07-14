#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int hist[256];
    
    // Initialize shared memory to zero
    if (threadIdx.x < 256) {
        hist[threadIdx.x] = 0;
    }
    __syncthreads();
    
    // Each thread processes elements in grid-stride loop
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        unsigned int bin = input[idx];
        atomicAdd(hist + bin, 1U);
    }
    __syncthreads();
    
    // Combine partial histogram to global memory using 256 threads to reduce contention
    if (threadIdx.x < 256) {
        unsigned int val = hist[threadIdx.x];
        if (val) {
            atomicAdd(output + threadIdx.x, val);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = (n + kBlock - 1) / kBlock;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        if (n > 0) {
            histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
        }
    }
}