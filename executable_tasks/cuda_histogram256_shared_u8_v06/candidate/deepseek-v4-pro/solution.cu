#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
    extern __shared__ unsigned int s_hist[];
    int tid = threadIdx.x;

    // Initialize shared memory histogram to zero
    for (int i = tid; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    while (idx < n) {
        unsigned char val = input[idx];
        atomicAdd(&s_hist[val], 1U);
        idx += stride;
    }
    __syncthreads();

    // Accumulate block histogram into global output
    for (int i = tid; i < 256; i += blockDim.x) {
        unsigned int val = s_hist[i];
        if (val > 0) {
            atomicAdd(output + i, val);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int blockSize = 256;
    int gridSize = (n + blockSize - 1) / blockSize;
    if (gridSize > 1024) gridSize = 1024;
    if (gridSize < 1) gridSize = 1;
    const size_t sharedMem = 256 * sizeof(unsigned int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_atomic_kernel<<<gridSize, blockSize, sharedMem>>>(input, output, n);
    }
}
