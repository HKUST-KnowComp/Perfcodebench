#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared memory histogram to zero
    if (threadIdx.x < 256) {
        s_hist[threadIdx.x] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    // Accumulate into shared memory
    for (; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1U);
    }
    __syncthreads();

    // Write shared results to global memory
    if (threadIdx.x < 256) {
        atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
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
