#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[256];
    
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int blockSize = blockDim.x;
    const int globalTid = bid * blockSize + tid;
    const int gridStride = gridDim.x * blockSize;
    
    // Initialize shared histogram
    if (tid < 256) {
        shist[tid] = 0;
    }
    __syncthreads();
    
    // Process 16 bytes at a time using uint4 (vectorized load)
    const int n16 = n / 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    
    for (int i = globalTid; i < n16; i += gridStride) {
        uint4 v = input4[i];
        
        // Extract all 16 bytes
        unsigned char b0  = (v.x) & 0xFF;
        unsigned char b1  = (v.x >> 8) & 0xFF;
        unsigned char b2  = (v.x >> 16) & 0xFF;
        unsigned char b3  = (v.x >> 24) & 0xFF;
        unsigned char b4  = (v.y) & 0xFF;
        unsigned char b5  = (v.y >> 8) & 0xFF;
        unsigned char b6  = (v.y >> 16) & 0xFF;
        unsigned char b7  = (v.y >> 24) & 0xFF;
        unsigned char b8  = (v.z) & 0xFF;
        unsigned char b9  = (v.z >> 8) & 0xFF;
        unsigned char b10 = (v.z >> 16) & 0xFF;
        unsigned char b11 = (v.z >> 24) & 0xFF;
        unsigned char b12 = (v.w) & 0xFF;
        unsigned char b13 = (v.w >> 8) & 0xFF;
        unsigned char b14 = (v.w >> 16) & 0xFF;
        unsigned char b15 = (v.w >> 24) & 0xFF;
        
        atomicAdd(&shist[b0], 1U);
        atomicAdd(&shist[b1], 1U);
        atomicAdd(&shist[b2], 1U);
        atomicAdd(&shist[b3], 1U);
        atomicAdd(&shist[b4], 1U);
        atomicAdd(&shist[b5], 1U);
        atomicAdd(&shist[b6], 1U);
        atomicAdd(&shist[b7], 1U);
        atomicAdd(&shist[b8], 1U);
        atomicAdd(&shist[b9], 1U);
        atomicAdd(&shist[b10], 1U);
        atomicAdd(&shist[b11], 1U);
        atomicAdd(&shist[b12], 1U);
        atomicAdd(&shist[b13], 1U);
        atomicAdd(&shist[b14], 1U);
        atomicAdd(&shist[b15], 1U);
    }
    
    // Handle remaining bytes
    const int remainder_start = n16 * 16;
    for (int i = remainder_start + globalTid; i < n; i += gridStride) {
        atomicAdd(&shist[input[i]], 1U);
    }
    
    __syncthreads();
    
    // Merge shared histogram to global
    for (int i = tid; i < 256; i += blockSize) {
        if (shist[i] > 0) {
            atomicAdd(&output[i], shist[i]);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 256;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}