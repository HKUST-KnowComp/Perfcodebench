#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Use shared memory for local histogram to reduce global atomic contention
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    local_hist[threadIdx.x] = 0;
    __syncthreads();

    const int tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = blockDim.x * gridDim.x;

    // Vectorized processing: read 4 bytes at a time to maximize memory throughput
    const uint4* input_v4 = reinterpret_cast<const uint4*>(input);
    int n_v4 = n / 16;
    
    for (int i = tid; i < n_v4; i += stride) {
        uint4 data = input_v4[i];
        
        // Process 16 bytes per iteration
        atomicAdd(&local_hist[(data.x >> 0) & 0xFF], 1);
        atomicAdd(&local_hist[(data.x >> 8) & 0xFF], 1);
        atomicAdd(&local_hist[(data.x >> 16) & 0xFF], 1);
        atomicAdd(&local_hist[(data.x >> 24) & 0xFF], 1);

        atomicAdd(&local_hist[(data.y >> 0) & 0xFF], 1);
        atomicAdd(&local_hist[(data.y >> 8) & 0xFF], 1);
        atomicAdd(&local_hist[(data.y >> 16) & 0xFF], 1);
        atomicAdd(&local_hist[(data.y >> 24) & 0xFF], 1);

        atomicAdd(&local_hist[(data.z >> 0) & 0xFF], 1);
        atomicAdd(&local_hist[(data.z >> 8) & 0xFF], 1);
        atomicAdd(&local_hist[(data.z >> 16) & 0xFF], 1);
        atomicAdd(&local_hist[(data.z >> 24) & 0xFF], 1);

        atomicAdd(&local_hist[(data.w >> 0) & 0xFF], 1);
        atomicAdd(&local_hist[(data.w >> 8) & 0xFF], 1);
        atomicAdd(&local_hist[(data.w >> 16) & 0xFF], 1);
        atomicAdd(&local_hist[(data.w >> 24) & 0xFF], 1);
    }

    // Handle remaining bytes if n is not a multiple of 16
    int processed = n_v4 * 16;
    for (int i = processed + tid; i < n; i += stride) {
        atomicAdd(&local_hist[input[i]], 1);
    }

    __syncthreads();

    // Merge local histogram into global memory
    atomicAdd(&output[threadIdx.x], local_hist[threadIdx.x]);
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int threads = 256;
    // Heuristic for grid size based on occupancy and input size
    const int blocks = (n + (threads * 16) - 1) / (threads * 16);
    const int grid = (blocks > 1024) ? 1024 : (blocks < 1 ? 1 : blocks);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, threads>>>(input, output, n);
    }
}