#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Use shared memory for local histogram to reduce global atomic contention
    // 256 bins * 4 bytes = 1024 bytes
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

    // Merge shared histogram into global output
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is generally efficient for this workload
    const int kBlock = 256;
    // Grid size tuned for high throughput across most modern GPUs
    const int grid = 512;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}