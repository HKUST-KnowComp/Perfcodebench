#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    // Each thread maintains a private histogram in shared memory
    // 64 threads * 256 bins = 16384 unsigned ints = 64KB shared memory
    // Use smaller approach: 32 threads with private histograms
    
    __shared__ unsigned int shist[32 * 256];  // 32KB shared memory
    
    const int tid = threadIdx.x;
    const int nthreads = blockDim.x;  // 256 threads
    const int warp_id = tid / 32;
    const int lane = tid & 31;
    
    // Initialize shared memory - each of 32 warps gets its own histogram
    // But we only have 8 warps (256/32), so use 8 private histograms
    // Actually use a different approach: sub-histograms per warp
    
    // Simpler approach: use 8 histograms (one per warp)
    // 8 * 256 = 2048 uints = 8KB
    __shared__ unsigned int whist[8 * 256];
    
    // Initialize warp's histogram
    unsigned int* my_hist = whist + warp_id * 256;
    #pragma unroll
    for (int i = lane; i < 256; i += 32) {
        my_hist[i] = 0;
    }
    __syncthreads();
    
    // Process input with vectorized loads (16 bytes at a time)
    const int gid = blockIdx.x * blockDim.x + tid;
    const int stride = blockDim.x * gridDim.x;
    
    // Vectorized processing - load 16 bytes at a time
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    const int n4 = n / 16;
    
    for (int i = gid; i < n4; i += stride) {
        uint4 data = input4[i];
        
        // Process 16 bytes from uint4
        unsigned char bytes[16];
        *reinterpret_cast<uint4*>(bytes) = data;
        
        #pragma unroll
        for (int j = 0; j < 16; j++) {
            atomicAdd(my_hist + bytes[j], 1U);
        }
    }
    
    // Handle remaining bytes
    const int base = n4 * 16;
    for (int i = base + gid; i < n; i += stride) {
        atomicAdd(my_hist + input[i], 1U);
    }
    
    __syncthreads();
    
    // Reduce 8 warp histograms into first histogram
    for (int bin = tid; bin < 256; bin += nthreads) {
        unsigned int sum = whist[bin];
        #pragma unroll
        for (int w = 1; w < 8; w++) {
            sum += whist[w * 256 + bin];
        }
        // Atomic add to global output
        atomicAdd(output + bin, sum);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 256;  // Fewer blocks, more work per block
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}