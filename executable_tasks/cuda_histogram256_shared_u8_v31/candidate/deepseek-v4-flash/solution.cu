#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int shared_hist[256];
    
    // Initialize shared memory to zero
    if (threadIdx.x < 256) {
        shared_hist[threadIdx.x] = 0;
    }
    __syncthreads();

    // Process elements with stride
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = tid; i < n; i += stride) {
        atomicAdd(&shared_hist[input[i]], 1U);
    }
    __syncthreads();

    // Merge block histogram into global output
    if (threadIdx.x < 256) {
        unsigned int bin = shared_hist[threadIdx.x];
        if (bin != 0) {
            atomicAdd(&output[threadIdx.x], bin);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
