#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared memory histogram
    s_hist[threadIdx.x] = 0;
    __syncthreads();
    
    int tid = threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1U);
    }
    __syncthreads();
    
    // Accumulate partial results to global memory
    atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int blocks = (n + kBlock - 1) / kBlock;
    if (blocks > 2048) blocks = 2048;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<blocks, kBlock>>>(input, output, n);
    }
}