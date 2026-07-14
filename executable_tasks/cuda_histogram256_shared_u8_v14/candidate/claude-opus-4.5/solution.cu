#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int NUM_PARTS = 32;  // Number of sub-histograms per block to avoid bank conflicts

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    // Shared memory: 32 sub-histograms of 256 bins each
    // Using 32 parts ensures different threads in a warp access different banks
    __shared__ unsigned int smem[NUM_PARTS * 256];
    
    // Initialize shared memory to zero
    // Each thread clears multiple locations
    for (int i = threadIdx.x; i < NUM_PARTS * 256; i += BLOCK_SIZE) {
        smem[i] = 0;
    }
    __syncthreads();
    
    // Each thread uses a sub-histogram based on threadIdx.x % 32
    // This ensures threads in the same warp use different sub-histograms
    const int part = threadIdx.x % NUM_PARTS;
    
    // Process input using vectorized loads (16 bytes at a time)
    const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    const int totalThreads = gridDim.x * BLOCK_SIZE;
    
    // Process 16 bytes per iteration using uint4
    const int n16 = n / 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    
    for (int i = tid; i < n16; i += totalThreads) {
        uint4 data = input4[i];
        
        // Extract and accumulate all 16 bytes
        unsigned int v0 = data.x;
        unsigned int v1 = data.y;
        unsigned int v2 = data.z;
        unsigned int v3 = data.w;
        
        // Process v0 (4 bytes)
        atomicAdd(&smem[part * 256 + (v0 & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v0 >> 8) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v0 >> 16) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + (v0 >> 24)], 1);
        
        // Process v1 (4 bytes)
        atomicAdd(&smem[part * 256 + (v1 & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v1 >> 8) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v1 >> 16) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + (v1 >> 24)], 1);
        
        // Process v2 (4 bytes)
        atomicAdd(&smem[part * 256 + (v2 & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v2 >> 8) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v2 >> 16) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + (v2 >> 24)], 1);
        
        // Process v3 (4 bytes)
        atomicAdd(&smem[part * 256 + (v3 & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v3 >> 8) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + ((v3 >> 16) & 0xFF)], 1);
        atomicAdd(&smem[part * 256 + (v3 >> 24)], 1);
    }
    
    // Handle remaining bytes
    const int remainder_start = n16 * 16;
    for (int i = remainder_start + tid; i < n; i += totalThreads) {
        atomicAdd(&smem[part * 256 + input[i]], 1);
    }
    
    __syncthreads();
    
    // Reduce sub-histograms and write to global memory
    // Each thread handles multiple bins
    for (int bin = threadIdx.x; bin < 256; bin += BLOCK_SIZE) {
        unsigned int sum = 0;
        for (int p = 0; p < NUM_PARTS; p++) {
            sum += smem[p * 256 + bin];
        }
        if (sum > 0) {
            atomicAdd(&output[bin], sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Use fewer blocks since each block now does more work with shared memory
    const int numSMs = 80;  // Typical for modern GPUs, will work on others too
    const int blocksPerSM = 2;
    const int grid = numSMs * blocksPerSM;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, BLOCK_SIZE>>>(input, output, n);
    }
}