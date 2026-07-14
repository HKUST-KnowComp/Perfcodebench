#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[256 * 32];  // 32 sub-histograms for 32 warps max
    
    const int tid = threadIdx.x;
    const int warpId = tid >> 5;
    const int laneId = tid & 31;
    const int numWarps = blockDim.x >> 5;
    
    // Initialize shared memory - each thread clears multiple locations
    for (int i = tid; i < 256 * numWarps; i += blockDim.x) {
        shist[i] = 0;
    }
    __syncthreads();
    
    // Pointer to this warp's histogram
    unsigned int* myHist = shist + warpId * 256;
    
    // Process input using vectorized loads
    const int elementsPerThread = 16;
    const int stride = blockDim.x * gridDim.x * elementsPerThread;
    int base = (blockIdx.x * blockDim.x + tid) * elementsPerThread;
    
    // Aligned vectorized processing
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    const int n4 = n >> 4;  // number of uint4 elements
    
    for (int i = blockIdx.x * blockDim.x + tid; i < n4; i += blockDim.x * gridDim.x) {
        uint4 data = input4[i];
        
        // Unpack and accumulate - x component
        atomicAdd(myHist + (data.x & 0xFF), 1);
        atomicAdd(myHist + ((data.x >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((data.x >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((data.x >> 24) & 0xFF), 1);
        
        // y component
        atomicAdd(myHist + (data.y & 0xFF), 1);
        atomicAdd(myHist + ((data.y >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((data.y >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((data.y >> 24) & 0xFF), 1);
        
        // z component
        atomicAdd(myHist + (data.z & 0xFF), 1);
        atomicAdd(myHist + ((data.z >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((data.z >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((data.z >> 24) & 0xFF), 1);
        
        // w component
        atomicAdd(myHist + (data.w & 0xFF), 1);
        atomicAdd(myHist + ((data.w >> 8) & 0xFF), 1);
        atomicAdd(myHist + ((data.w >> 16) & 0xFF), 1);
        atomicAdd(myHist + ((data.w >> 24) & 0xFF), 1);
    }
    
    // Handle remaining elements
    int tailStart = n4 << 4;
    for (int i = tailStart + blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        atomicAdd(myHist + input[i], 1);
    }
    
    __syncthreads();
    
    // Reduce all warp histograms to global memory
    for (int bin = tid; bin < 256; bin += blockDim.x) {
        unsigned int sum = 0;
        for (int w = 0; w < numWarps; ++w) {
            sum += shist[w * 256 + bin];
        }
        if (sum > 0) {
            atomicAdd(output + bin, sum);
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