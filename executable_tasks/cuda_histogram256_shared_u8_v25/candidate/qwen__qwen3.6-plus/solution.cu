#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int s_hist[256];
    if (threadIdx.x < 256) s_hist[threadIdx.x] = 0;
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1U);
    }
    __syncthreads();

    if (threadIdx.x < 256) {
        atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 512;
    const int grid = (n + kBlock - 1) / kBlock;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
