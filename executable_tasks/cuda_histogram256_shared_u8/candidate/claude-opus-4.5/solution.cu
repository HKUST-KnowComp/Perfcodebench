#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int NUM_PARTS = 32;  // 32 sub-histograms to avoid bank conflicts

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[NUM_PARTS * 256];

    // Initialize shared memory - each thread clears multiple locations
    for (int i = threadIdx.x; i < NUM_PARTS * 256; i += BLOCK_SIZE) {
        shist[i] = 0;
    }
    __syncthreads();

    // Each thread uses a sub-histogram based on threadIdx.x % NUM_PARTS
    // This distributes accesses across banks
    const int part = threadIdx.x % NUM_PARTS;
    unsigned int* myHist = shist + part * 256;

    // Process 16 bytes at a time using vectorized loads
    int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    int stride = gridDim.x * BLOCK_SIZE;
    
    // Vectorized path: process uint4 (16 bytes) at a time
    int n16 = n / 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    
    for (int i = tid; i < n16; i += stride) {
        uint4 v = input4[i];
        
        // Unpack and accumulate - extract bytes from each uint
        unsigned int a = v.x;
        unsigned int b = v.y;
        unsigned int c = v.z;
        unsigned int d = v.w;
        
        atomicAdd(myHist + (a & 0xFF), 1);
        atomicAdd(myHist + ((a >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((a >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((a >> 24) & 0xFF), 1);
        
        atomicAdd(myHist + (b & 0xFF), 1);
        atomicAdd(myHist + ((b >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((b >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((b >> 24) & 0xFF), 1);
        
        atomicAdd(myHist + (c & 0xFF), 1);
        atomicAdd(myHist + ((c >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((c >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((c >> 24) & 0xFF), 1);
        
        atomicAdd(myHist + (d & 0xFF), 1);
        atomicAdd(myHist + ((d >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((d >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((d >> 24) & 0xFF), 1);
    }
    
    // Handle remaining bytes
    int base = n16 * 16;
    for (int i = base + tid; i < n; i += stride) {
        atomicAdd(myHist + input[i], 1);
    }
    
    __syncthreads();

    // Reduce sub-histograms and write to global memory
    // Each thread handles multiple bins
    for (int bin = threadIdx.x; bin < 256; bin += BLOCK_SIZE) {
        unsigned int sum = 0;
        for (int p = 0; p < NUM_PARTS; p++) {
            sum += shist[p * 256 + bin];
        }
        if (sum > 0) {
            atomicAdd(output + bin, sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int grid = 256;  // Fewer blocks with more work per block
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, BLOCK_SIZE>>>(input, output, n);
    }
}