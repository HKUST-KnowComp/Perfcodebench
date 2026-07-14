#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* output, int n) {
    __shared__ unsigned int hist[256];
    // initialize shared memory histogram to zero
    if (threadIdx.x < 256) {
        hist[threadIdx.x] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = gridDim.x * blockDim.x;
    for (; idx < n; idx += stride) {
        unsigned char val = input[idx];
        atomicAdd(&hist[val], 1U);
    }
    __syncthreads();

    // write block histogram to global output
    if (threadIdx.x < 256) {
        atomicAdd(output + threadIdx.x, hist[threadIdx.x]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    if (grid < 1) grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
