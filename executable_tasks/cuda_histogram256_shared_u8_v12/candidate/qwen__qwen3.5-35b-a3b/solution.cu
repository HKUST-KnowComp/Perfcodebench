#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    // Initialize shared memory
    for (int i = tid; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Process elements
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        s_hist[input[i]]++;
    }
    __syncthreads();

    // Atomically add to global output
    if (tid == 0) {
        for (int i = 0; i < 256; i++) {
            atomicAdd(output + i, s_hist[i]);
        }
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