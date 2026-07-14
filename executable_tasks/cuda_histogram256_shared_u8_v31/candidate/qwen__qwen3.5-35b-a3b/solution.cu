#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int shared_hist[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Initialize shared histogram
    if (tid < 256) {
        shared_hist[tid] = 0;
    }
    __syncthreads();

    // Process data
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        unsigned char val = input[i];
        atomicAdd(&shared_hist[val], 1U);
    }
    __syncthreads();

    // Merge to global
    if (tid < 256) {
        atomicAdd(&output[tid], shared_hist[tid]);
    }
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