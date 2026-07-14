#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram256_kernel(const unsigned char* input, unsigned int* output, int n) {
    extern __shared__ unsigned int hist_s[];
    // Initialize shared memory histogram to zero
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        hist_s[i] = 0;
    }
    __syncthreads();

    // Accumulate histogram using shared atomics
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        atomicAdd(&hist_s[input[idx]], 1U);
    }
    __syncthreads();

    // Write partial histogram to global memory with atomic adds
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], hist_s[i]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int block_dim = 256;
    // Compute grid size to cover all elements, capped to avoid excessive blocks
    int grid = (n + block_dim - 1) / block_dim;
    if (grid > 8192) grid = 8192;
    // Ensure at least one block if n>0, but if n==0 grid=0; launch would fail. So clamp to 1.
    if (grid == 0) grid = 1;

    const size_t shared_mem_size = 256 * sizeof(unsigned int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_kernel<<<grid, block_dim, shared_mem_size>>>(input, output, n);
    }
}
