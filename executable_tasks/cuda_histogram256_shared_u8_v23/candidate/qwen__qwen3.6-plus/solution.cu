#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int s_hist[256];
    const int tid = threadIdx.x;
    const int stride = blockDim.x;

    for (int i = tid; i < 256; i += stride) {
        s_hist[i] = 0;
    }
    __syncthreads();

    for (int idx = blockIdx.x * stride + tid; idx < n; idx += stride * gridDim.x) {
        unsigned char val = __ldg(&input[idx]);
        atomicAdd(&s_hist[val], 1U);
    }
    __syncthreads();

    for (int i = tid; i < 256; i += stride) {
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