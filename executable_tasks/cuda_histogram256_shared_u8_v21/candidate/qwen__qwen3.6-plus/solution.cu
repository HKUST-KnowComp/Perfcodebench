#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int s_hist[256];
    int tid = threadIdx.x;
    
    // Initialize shared memory histogram to zero
    if (tid < 256) {
        s_hist[tid] = 0;
    }
    __syncthreads();
    
    // Grid-stride loop to process input
    int stride = blockDim.x * gridDim.x;
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += stride) {
        s_hist[input[idx]]++;
    }
    __syncthreads();
    
    // Atomically add shared histogram to global output
    if (tid < 256) {
        atomicAdd(&output[tid], s_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    if (grid == 0) grid = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
