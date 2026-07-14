#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Local histogram in shared memory to reduce global atomic contention
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements using a grid-stride loop
    // We use a larger loop to ensure all elements are covered
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // To avoid massive shared memory usage per block, we use a 256-bin local buffer.
    // Since 256 * 4 bytes = 1KB, this fits easily in shared memory.
    // However, to handle the case where threads > 256, we use the thread index
    // to map to the local histogram if the block size is 256.
    
    // Note: The kernel is launched with blockDim.x = 256.
    // We use a local histogram per block.
    
    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Reduce local histogram to global output
    // Each thread handles one bin to minimize global atomic contention
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using 256 threads per block to match the 256 bins for easy reduction
    constexpr int kBlock = 256;
    // Calculate grid size to cover n elements efficiently
    // We use a fixed grid size to keep launch overhead consistent, but ensure it's sufficient
    // or use a dynamic grid. For performance, a large enough grid is better.
    const int kGrid = 256; 

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        // We use a grid-stride loop inside the kernel, so grid size doesn't strictly need to be n/kBlock
        // but we want enough parallelism.
        histogram256_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}