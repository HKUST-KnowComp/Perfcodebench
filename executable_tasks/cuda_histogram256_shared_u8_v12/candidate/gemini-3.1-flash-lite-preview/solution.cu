#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    __shared__ unsigned int smem[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        smem[i] = 0;
    }
    __syncthreads();

    // Process data in chunks
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        atomicAdd(&smem[input[i]], 1);
    }

    __syncthreads();

    // Merge shared memory into global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], smem[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int block_size = 256;
    const int grid_size = 128; // Balanced for occupancy and contention

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid_size, block_size>>>(input, output, n);
    }
}