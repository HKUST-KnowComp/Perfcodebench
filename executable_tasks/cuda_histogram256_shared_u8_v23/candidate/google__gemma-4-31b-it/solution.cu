#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Use shared memory for local histogram to reduce global atomic contention
    // Padding to 256 + 16 to help avoid potential bank conflicts, though 256 is power of 2
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Process input data
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = tid; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1);
    }
    __syncthreads();

    // Aggregate shared memory results to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Tuning parameters: 256 threads per block is generally efficient
    const int kBlock = 256;
    // Calculate grid size to saturate the GPU
    // 256 * 1024 = 262,144 threads, usually enough for most GPUs
    const int grid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}