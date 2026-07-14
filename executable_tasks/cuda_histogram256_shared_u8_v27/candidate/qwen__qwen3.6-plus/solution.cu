#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int s_hist[256];
    int tid = threadIdx.x;

    // Initialize shared memory histogram to zero
    for (int i = tid; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Grid-stride loop over input data
    int stride = blockDim.x * gridDim.x;
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += stride) {
        unsigned char val = __ldg(&input[idx]);
        atomicAdd(&s_hist[val], 1U);
    }
    __syncthreads();

    // Accumulate shared histogram into global output
    for (int i = tid; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
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