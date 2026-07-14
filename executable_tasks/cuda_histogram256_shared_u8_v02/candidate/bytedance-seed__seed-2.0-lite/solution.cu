#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    // Initialize per-block shared histogram to zero
    if (threadIdx.x < 256) {
        s_hist[threadIdx.x] = 0;
    }
    __syncthreads();

    // Process input with vectorized 32-bit loads to maximize memory throughput
    const unsigned int* input_vec = reinterpret_cast<const unsigned int*>(input);
    const int total_vec = n / 4;
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < total_vec; idx += blockDim.x * gridDim.x) {
        const unsigned int packed = input_vec[idx];
        const unsigned char b0 = static_cast<unsigned char>(packed & 0xFF);
        const unsigned char b1 = static_cast<unsigned char>((packed >> 8) & 0xFF);
        const unsigned char b2 = static_cast<unsigned char>((packed >> 16) & 0xFF);
        const unsigned char b3 = static_cast<unsigned char>((packed >> 24) & 0xFF);
        atomicAdd(&s_hist[b0], 1U);
        atomicAdd(&s_hist[b1], 1U);
        atomicAdd(&s_hist[b2], 1U);
        atomicAdd(&s_hist[b3], 1U);
    }

    // Process any remaining non-vectorized input bytes
    const int rem_start = total_vec * 4;
    for (int idx = rem_start + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        const unsigned char b = input[idx];
        atomicAdd(&s_hist[b], 1U);
    }

    __syncthreads();
    // Merge block's shared histogram into global output
    if (threadIdx.x < 256) {
        atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_atomic_kernel<<<grid, kBlock>>>(input, output, n);
    }
}