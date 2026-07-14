#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int shared_hist[256];
    
    // Initialize shared histogram to zero
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        shared_hist[i] = 0;
    }
    __syncthreads();

    // Vectorized load: process 4 bytes at a time
    int idx = (blockIdx.x * blockDim.x + threadIdx.x) * 4;
    int stride = blockDim.x * gridDim.x * 4;
    
    // Main loop using uchar4
    while (idx + 3 < n) {
        uchar4 data = reinterpret_cast<const uchar4*>(input)[idx / 4];
        atomicAdd(&shared_hist[data.x], 1U);
        atomicAdd(&shared_hist[data.y], 1U);
        atomicAdd(&shared_hist[data.z], 1U);
        atomicAdd(&shared_hist[data.w], 1U);
        idx += stride;
    }
    
    // Handle remaining elements (0-3)
    for (; idx < n; ++idx) {
        atomicAdd(&shared_hist[input[idx]], 1U);
    }
    __syncthreads();

    // Each thread atomically adds one bin to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], shared_hist[i]);
    }
}

}  // anonymous namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    // Compute grid: cover all elements but cap at 4096 blocks to limit launch overhead
    int grid = (n + kBlock * 4 - 1) / (kBlock * 4);  // each thread processes 4 elements per stride
    grid = min(grid, 4096);
    grid = max(grid, 1);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
