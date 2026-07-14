#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256];
    
    // Initialize shared histogram
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    // Process 4 bytes at a time for better memory throughput
    int n4 = n & ~3;
    const uchar4* input4 = reinterpret_cast<const uchar4*>(input);
    
    for (int i = idx; i < n4 / 4; i += stride) {
        uchar4 v = input4[i];
        atomicAdd(&s_hist[v.x], 1U);
        atomicAdd(&s_hist[v.y], 1U);
        atomicAdd(&s_hist[v.z], 1U);
        atomicAdd(&s_hist[v.w], 1U);
    }
    
    // Handle remaining bytes
    for (int i = n4 + idx; i < n; i += stride) {
        atomicAdd(&s_hist[input[i]], 1U);
    }
    
    __syncthreads();
    
    // Merge shared histogram into global output
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        unsigned int val = s_hist[i];
        if (val != 0) {
            atomicAdd(&output[i], val);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
