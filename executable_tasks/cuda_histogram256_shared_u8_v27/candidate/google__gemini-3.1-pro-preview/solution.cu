#include "interface.h"
#include <cuda_runtime.h>
#include <stdint.h>

namespace {

__global__ void histogram_shared_kernel(const uint8_t* __restrict__ input, uint32_t* __restrict__ output, int n) {
    // Use shared memory for local histogram to reduce global atomic contention
    __shared__ uint32_t local_hist[256];

    // Initialize shared memory
    local_hist[threadIdx.x] = 0;
    __syncthreads();

    const int tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = blockDim.x * gridDim.x;

    // Vectorized processing: read 4 bytes at a time
    const uint32_t* input_u32 = reinterpret_cast<const uint32_t*>(input);
    int n_u32 = n / 4;
    
    for (int i = tid; i < n_u32; i += stride) {
        uint32_t data = input_u32[i];
        atomicAdd(&local_hist[data & 0xFF], 1U);
        atomicAdd(&local_hist[(data >> 8) & 0xFF], 1U);
        atomicAdd(&local_hist[(data >> 16) & 0xFF], 1U);
        atomicAdd(&local_hist[(data >> 24) & 0xFF], 1U);
    }

    // Handle remaining bytes if n is not a multiple of 4
    if (tid == 0) {
        for (int i = n_u32 * 4; i < n; ++i) {
            atomicAdd(&local_hist[input[i]], 1U);
        }
    }

    __syncthreads();

    // Aggregate local histogram into global memory
    atomicAdd(&output[threadIdx.x], local_hist[threadIdx.x]);
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Tuning parameters: 256 threads per block matches the 256 bins
    const int block_size = 256;
    // Grid size tuned for typical GPU occupancy
    int grid_size = (n + (block_size * 4) - 1) / (block_size * 4);
    if (grid_size > 1024) grid_size = 1024;
    if (grid_size < 1) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        if (n > 0) {
            histogram_shared_kernel<<<grid_size, block_size>>>(input, output, n);
        }
    }
}