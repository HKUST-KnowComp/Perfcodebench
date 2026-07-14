#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    int tid = threadIdx.x;
    
    // Initialize shared histogram to zero
    if (tid < 256) {
        s_hist[tid] = 0;
    }
    __syncthreads();
    
    // Each thread processes a strided segment of the input
    int stride = blockDim.x * gridDim.x;
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1U);
    }
    __syncthreads();
    
    // Each thread writes its bin to global memory
    if (tid < 256) {
        atomicAdd(&output[tid], s_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid == 0) grid = 1;
    if (grid > 2048) grid = 2048;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
