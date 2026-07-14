#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Local histogram in shared memory
    // 256 bins * 4 bytes = 1024 bytes. Fits easily in shared memory.
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Grid-stride loop for processing input
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&s_hist[val], 1U);
    }
    __syncthreads();

    // Aggregate shared memory to global memory
    // Each thread handles a subset of the 256 bins to reduce atomic contention
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using 256 threads per block to match the bin count for efficient shared memory init/reduction
    constexpr int kBlock = 256;
    // A grid size that balances occupancy and atomic contention on the global output
    // 128-256 blocks is usually a sweet spot for this workload
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}