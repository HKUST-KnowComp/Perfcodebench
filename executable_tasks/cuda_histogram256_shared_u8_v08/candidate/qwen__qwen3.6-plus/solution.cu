#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];

    // Initialize shared histogram
    if (threadIdx.x < 256) {
        s_hist[threadIdx.x] = 0;
    }
    __syncthreads();

    // Grid-stride loop over input
    int stride = blockDim.x * gridDim.x;
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1);
    }
    __syncthreads();

    // Merge shared histogram to global memory
    if (threadIdx.x < 256) {
        atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 256) grid = 256;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
