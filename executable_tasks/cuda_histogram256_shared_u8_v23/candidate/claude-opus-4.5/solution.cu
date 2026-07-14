#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    // 32 sub-histograms to avoid bank conflicts (32 banks in shared memory)
    __shared__ unsigned int shist[32][256];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    
    // Initialize shared memory - each thread clears multiple entries
    #pragma unroll
    for (int i = tid; i < 32 * 256; i += blockDim.x) {
        int sub = i >> 8;        // i / 256
        int bin = i & 255;       // i % 256
        shist[sub][bin] = 0;
    }
    __syncthreads();
    
    // Process input using vectorized loads (16 bytes at a time)
    const int n16 = n >> 4;  // number of uint4 elements
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n16; idx += blockDim.x * gridDim.x) {
        uint4 data = input4[idx];
        
        // Extract and process all 16 bytes
        unsigned char bytes[16];
        bytes[0]  = data.x & 0xFF;
        bytes[1]  = (data.x >> 8) & 0xFF;
        bytes[2]  = (data.x >> 16) & 0xFF;
        bytes[3]  = (data.x >> 24) & 0xFF;
        bytes[4]  = data.y & 0xFF;
        bytes[5]  = (data.y >> 8) & 0xFF;
        bytes[6]  = (data.y >> 16) & 0xFF;
        bytes[7]  = (data.y >> 24) & 0xFF;
        bytes[8]  = data.z & 0xFF;
        bytes[9]  = (data.z >> 8) & 0xFF;
        bytes[10] = (data.z >> 16) & 0xFF;
        bytes[11] = (data.z >> 24) & 0xFF;
        bytes[12] = data.w & 0xFF;
        bytes[13] = (data.w >> 8) & 0xFF;
        bytes[14] = (data.w >> 16) & 0xFF;
        bytes[15] = (data.w >> 24) & 0xFF;
        
        #pragma unroll
        for (int i = 0; i < 16; ++i) {
            atomicAdd(&shist[lane][bytes[i]], 1U);
        }
    }
    
    // Handle remaining elements (tail)
    const int tail_start = n16 << 4;
    for (int idx = tail_start + blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        unsigned char val = input[idx];
        atomicAdd(&shist[lane][val], 1U);
    }
    
    __syncthreads();
    
    // Reduce shared histograms to global memory
    for (int bin = tid; bin < 256; bin += blockDim.x) {
        unsigned int sum = 0;
        #pragma unroll
        for (int sub = 0; sub < 32; ++sub) {
            sum += shist[sub][bin];
        }
        if (sum > 0) {
            atomicAdd(&output[bin], sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 256;  // Fewer blocks since each does more work
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}