#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Use shared memory for local histogram bins
    // 256 bins * 4 bytes = 1024 bytes
    __shared__ unsigned int s_bins[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_bins[i] = 0;
    }
    __syncthreads();

    // Local accumulation
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = tid; idx < n; idx += stride) {
        atomicAdd(&s_bins[input[idx]], 1);
    }
    __syncthreads();

    // Global aggregation
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_bins[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Tuning parameters
    const int kBlockSize = 256;
    // Calculate grid size to saturate GPU while keeping shared memory usage reasonable
    // 1024 blocks * 256 threads = 262,144 threads
    const int kGridSize = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<kGridSize, kBlockSize>>>(input, output, n);
    }
}