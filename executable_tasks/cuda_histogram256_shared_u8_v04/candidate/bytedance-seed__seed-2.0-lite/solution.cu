#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void histogram_shared_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize per-block shared histogram to zero
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();
    
    // Process input in vectorized 4-byte chunks to maximize memory bandwidth
    const int main_end = n / 4 * 4;
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < main_end; idx += blockDim.x * gridDim.x) {
        const uint4 data = reinterpret_cast<const uint4*>(input)[idx / 4];
        atomicAdd(&s_hist[data.x], 1U);
        atomicAdd(&s_hist[data.y], 1U);
        atomicAdd(&s_hist[data.z], 1U);
        atomicAdd(&s_hist[data.w], 1U);
    }
    
    // Handle any remaining elements not divisible by 4 for full input coverage
    if (main_end + threadIdx.x < n) {
        const unsigned char val = input[main_end + threadIdx.x];
        atomicAdd(&s_hist[val], 1U);
    }
    __syncthreads();
    
    // Merge block's local histogram into the global output array
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}
} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 1024;
    const int grid = 2048; // Sufficient to saturate all SMs on modern CUDA GPUs
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_atomic_kernel<<<grid, kBlock>>>(input, output, n);
    }
}