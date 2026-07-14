#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdint.h>

namespace {

constexpr int kThreads = 256;
constexpr int kSubHists = 4; // Number of sub-histograms in shared memory to reduce contention

__global__ void histogram_kernel_optimized(const uint4* __restrict__ input, unsigned int* __restrict__ output, int n_uint4, int n_total) {
    // Shared memory for local histograms
    __shared__ unsigned int s_hist[kSubHists][256];

    // Initialize shared memory
    #pragma unroll
    for (int i = 0; i < kSubHists; ++i) {
        s_hist[i][threadIdx.x] = 0;
    }
    __syncthreads();

    // Process data in uint4 chunks (16 bytes per load)
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int sub_idx = threadIdx.x % kSubHists;

    for (int i = idx; i < n_uint4; i += stride) {
        uint4 data = input[i];

        // Process 16 bytes
        atomicAdd(&s_hist[sub_idx][(data.x >> 0) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.x >> 8) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.x >> 16) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.x >> 24) & 0xFF], 1);

        atomicAdd(&s_hist[sub_idx][(data.y >> 0) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.y >> 8) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.y >> 16) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.y >> 24) & 0xFF], 1);

        atomicAdd(&s_hist[sub_idx][(data.z >> 0) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.z >> 8) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.z >> 16) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.z >> 24) & 0xFF], 1);

        atomicAdd(&s_hist[sub_idx][(data.w >> 0) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.w >> 8) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.w >> 16) & 0xFF], 1);
        atomicAdd(&s_hist[sub_idx][(data.w >> 24) & 0xFF], 1);
    }

    // Handle remaining bytes if n is not a multiple of 16
    if (blockIdx.x == 0 && threadIdx.x == 0) {
        const unsigned char* input_bytes = reinterpret_cast<const unsigned char*>(input);
        for (int i = n_uint4 * 16; i < n_total; ++i) {
            atomicAdd(&s_hist[0][input_bytes[i]], 1);
        }
    }

    __syncthreads();

    // Reduce shared memory histograms to global memory
    unsigned int count = 0;
    #pragma unroll
    for (int i = 0; i < kSubHists; ++i) {
        count += s_hist[i][threadIdx.x];
    }
    if (count > 0) {
        atomicAdd(&output[threadIdx.x], count);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int n_uint4 = n / 16;
    
    // Heuristic for grid size based on occupancy
    int dev_id = 0;
    cudaGetDevice(&dev_id);
    int num_sm = 0;
    cudaDeviceGetAttribute(&num_sm, cudaDevAttrMultiProcessorCount, dev_id);
    
    const int grid = num_sm * 4; // Aim for high occupancy

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_kernel_optimized<<<grid, kThreads>>>(reinterpret_cast<const uint4*>(input), output, n_uint4, n);
    }
}