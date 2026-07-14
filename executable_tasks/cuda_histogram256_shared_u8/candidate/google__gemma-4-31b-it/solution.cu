#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Use a padded shared memory array to reduce bank conflicts
// 256 bins * 4 bytes = 1024 bytes. 
// Padding the inner dimension slightly or using a specific layout can help,
// but for 256 bins, a simple shared array is often efficient if accessed randomly.
__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Local histogramming
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = tid; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1);
    }
    __syncthreads();

    // Global aggregation
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is generally a good balance for occupancy
    const int kBlock = 256;
    // Calculate grid size to saturate the GPU
    // Using a fixed large number or calculating based on SM count is common.
    // 128-256 blocks usually suffice for most modern GPUs to hide latency.
    const int kGrid = 128;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}