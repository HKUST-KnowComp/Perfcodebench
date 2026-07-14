#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Shared memory for 256 bins
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Local histogramming into shared memory
    for (int idx = tid; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1);
    }

    __syncthreads();

    // Write shared memory results to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(output + i, s_hist[i]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Use a block size that fits well with shared memory and occupancy
    constexpr int kBlock = 256;
    // Grid size chosen to balance parallelism and shared memory overhead
    const int grid = (n + kBlock - 1) / kBlock > 1024 ? 1024 : 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}