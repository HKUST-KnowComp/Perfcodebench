#include "interface.h"
#include <cuda_runtime.h>
#include <stdint.h>

namespace {

__global__ void histogram_shared_atomic_kernel(const uint32_t* input, unsigned int* output, int n_words, int n_total) {
    // Each block has its own local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    local_hist[threadIdx.x] = 0;
    __syncthreads();

    // Process input in 4-byte chunks
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n_words; i += stride) {
        uint32_t data = input[i];
        atomicAdd(&local_hist[data & 0xFF], 1);
        atomicAdd(&local_hist[(data >> 8) & 0xFF], 1);
        atomicAdd(&local_hist[(data >> 16) & 0xFF], 1);
        atomicAdd(&local_hist[(data >> 24) & 0xFF], 1);
    }

    // Handle remaining bytes if n is not a multiple of 4
    if (idx == 0) {
        int processed_bytes = n_words * 4;
        const unsigned char* input_bytes = (const unsigned char*)input;
        for (int i = processed_bytes; i < n_total; ++i) {
            atomicAdd(&local_hist[input_bytes[i]], 1);
        }
    }

    __syncthreads();

    // Accumulate local histogram into global memory
    atomicAdd(&output[threadIdx.x], local_hist[threadIdx.x]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int block_size = 256;
    // Use a grid size that balances occupancy and shared memory reduction overhead
    const int grid_size = (n < 1024 * 256) ? (n + block_size - 1) / block_size : 512;
    
    int n_words = n / 4;
    const uint32_t* input_u32 = reinterpret_cast<const uint32_t*>(input);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        if (n > 0) {
            histogram_shared_atomic_kernel<<<grid_size, block_size>>>(input_u32, output, n_words, n);
        }
    }
}