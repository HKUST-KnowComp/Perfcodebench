#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared memory histogram to zero
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    // Each thread processes a strided segment of the input
    for (; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1U);
    }
    __syncthreads();
    
    // Each thread atomically adds its bin's count to the global output
    if (threadIdx.x < 256) {
        atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid == 0) grid = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}