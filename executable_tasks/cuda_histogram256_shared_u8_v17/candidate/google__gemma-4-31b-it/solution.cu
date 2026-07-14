#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Shared memory for 256 bins (256 * 4 bytes = 1KB)
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
        s_hist[input[idx]]++;
    }
    __syncthreads();

    // Flush shared memory to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(output + i, s_hist[i]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Use a block size that balances occupancy and shared memory usage
    // 256 threads per block is generally efficient for this workload
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU
    const int grid = (n + kBlock - 1) / kBlock;
    // Cap grid size to avoid excessive overhead on very large n
    const int capped_grid = (grid > 1024) ? 1024 : grid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<capped_grid, kBlock>>>(input, output, n);
    }
}