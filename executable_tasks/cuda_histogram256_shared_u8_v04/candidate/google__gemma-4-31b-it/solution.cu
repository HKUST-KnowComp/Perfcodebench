#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Use shared memory for local histogram to reduce global memory contention
    // 256 bins * 4 bytes = 1024 bytes
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Local accumulation
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

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is generally efficient for this workload
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU
    // 1024 blocks * 256 threads = 262,144 threads
    const int grid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}