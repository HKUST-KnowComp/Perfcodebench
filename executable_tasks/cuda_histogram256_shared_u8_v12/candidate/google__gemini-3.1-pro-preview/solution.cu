#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void histogram_optimized_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Shared memory for local histogram (256 bins * 4 bytes = 1KB)
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Grid-stride loop with vectorized loads
    int idx = blockIdx.x * blockDim.x + tid;
    int stride = blockDim.x * gridDim.x;

    // Process data in 16-byte chunks using uint4
    const uint4* input_v4 = reinterpret_cast<const uint4*>(input);
    int n_v4 = n / 16;

    for (int i = idx; i < n_v4; i += stride) {
        uint4 data = input_v4[i];
        
        // Unroll processing of 16 bytes
        atomicAdd(&local_hist[(data.x >> 0) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.x >> 8) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.x >> 16) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.x >> 24) & 0xFF], 1U);

        atomicAdd(&local_hist[(data.y >> 0) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.y >> 8) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.y >> 16) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.y >> 24) & 0xFF], 1U);

        atomicAdd(&local_hist[(data.z >> 0) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.z >> 8) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.z >> 16) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.z >> 24) & 0xFF], 1U);

        atomicAdd(&local_hist[(data.w >> 0) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.w >> 8) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.w >> 16) & 0xFF], 1U);
        atomicAdd(&local_hist[(data.w >> 24) & 0xFF], 1U);
    }

    // Handle remaining bytes
    int remainder_start = n_v4 * 16;
    for (int i = remainder_start + idx; i < n; i += stride) {
        atomicAdd(&local_hist[input[i]], 1U);
    }

    __syncthreads();

    // Merge local histogram into global memory
    if (tid < 256) {
        unsigned int val = local_hist[tid];
        if (val > 0) {
            atomicAdd(&output[tid], val);
        }
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is ideal for 256-bin histogram shared memory initialization/reduction
    const int block_size = 256;
    // Choose grid size to saturate the GPU without excessive overhead in the final reduction
    // 160-320 blocks is usually a sweet spot for modern GPUs
    const int grid_size = 320;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_optimized_kernel<<<grid_size, block_size>>>(input, output, n);
    }
}