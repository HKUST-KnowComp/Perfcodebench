#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int s_hist[256];
    int tid = threadIdx.x;
    
    // Initialize shared histogram
    for (int i = tid; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Process input
    int idx = blockIdx.x * blockDim.x + tid;
    int stride = blockDim.x * gridDim.x;
    for (; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1U);
    }
    __syncthreads();

    // Write to global
    atomicAdd(&output[tid], s_hist[tid]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid == 0) grid = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
