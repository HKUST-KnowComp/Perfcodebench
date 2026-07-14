#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Use shared memory to reduce global atomic contention
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Process input in strides
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Vectorized loads (4 bytes at a time) to improve memory throughput
    const uint4* input_v4 = reinterpret_cast<const uint4*>(input);
    int n_v4 = n / 16;
    int idx_v4 = blockIdx.x * blockDim.x + threadIdx.x;
    int stride_v4 = blockDim.x * gridDim.x;

    for (int i = idx_v4; i < n_v4; i += stride_v4) {
        uint4 data = input_v4[i];
        
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

    // Handle remaining bytes
    for (int i = n_v4 * 16 + idx; i < n; i += stride) {
        atomicAdd(&local_hist[input[i]], 1);
    }

    __syncthreads();

    // Merge shared memory histogram into global memory
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int threads = 256;
    // Calculate grid size based on hardware occupancy or fixed heuristic
    const int blocks = (n + (threads * 16) - 1) / (threads * 16) > 1024 ? 1024 : (n + (threads * 16) - 1) / (threads * 16);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        if (n > 0) {
            histogram_shared_kernel<<<blocks > 0 ? blocks : 1, threads>>>(input, output, n);
        }
    }
}