#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Use a padded shared memory array to reduce bank conflicts
// 256 bins * 4 bytes = 1024 bytes. 
// We use a structure that fits in shared memory and allows efficient access.
__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Shared memory for 256 bins. 
    // Using a fixed size array. 256 * 4 bytes = 1KB, well within limits.
    __shared__ unsigned int s_bins[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        s_bins[tid] = 0;
    }
    __syncthreads();

    // Grid-stride loop to process input
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (idx < n) {
        unsigned char val = input[idx];
        atomicAdd(&s_bins[val], 1);
        idx += stride;
    }
    __syncthreads();

    // Aggregate shared memory bins into global memory
    // Each thread handles one bin to parallelize the final atomicAdd
    if (tid < 256) {
        atomicAdd(&output[tid], s_bins[tid]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // 256 threads per block to map 1:1 with bins for the final reduction
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU
    // 1024 blocks is usually sufficient for most modern GPUs
    const int grid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}