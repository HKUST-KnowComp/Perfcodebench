#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlock = 256;
constexpr int kSubHists = 32;

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[kSubHists * 256];

    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warpId = tid >> 5;

    // Initialize shared memory - each thread clears 32 elements
    #pragma unroll
    for (int i = 0; i < kSubHists; ++i) {
        shist[i * 256 + tid] = 0;
    }
    __syncthreads();

    // Process input using vectorized loads (16 bytes at a time)
    const int tid_global = blockIdx.x * blockDim.x + tid;
    const int stride = blockDim.x * gridDim.x;
    
    // Vectorized path: process 16 bytes per iteration
    const int n16 = (n / 16) * 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    const int n4 = n16 / 16;
    
    for (int i = tid_global; i < n4; i += stride) {
        uint4 v = input4[i];
        
        unsigned int v0 = v.x;
        unsigned int v1 = v.y;
        unsigned int v2 = v.z;
        unsigned int v3 = v.w;
        
        // Extract and accumulate bytes - use lane as sub-histogram index
        #pragma unroll
        for (int b = 0; b < 4; ++b) {
            unsigned char c0 = (v0 >> (b * 8)) & 0xFF;
            unsigned char c1 = (v1 >> (b * 8)) & 0xFF;
            unsigned char c2 = (v2 >> (b * 8)) & 0xFF;
            unsigned char c3 = (v3 >> (b * 8)) & 0xFF;
            
            atomicAdd(&shist[lane * 256 + c0], 1);
            atomicAdd(&shist[lane * 256 + c1], 1);
            atomicAdd(&shist[lane * 256 + c2], 1);
            atomicAdd(&shist[lane * 256 + c3], 1);
        }
    }
    
    // Handle remaining elements
    for (int i = n16 + tid_global; i < n; i += stride) {
        unsigned char c = input[i];
        atomicAdd(&shist[lane * 256 + c], 1);
    }
    
    __syncthreads();

    // Reduce sub-histograms: each thread handles one bin value
    // Thread tid handles bin 'tid' across all 32 sub-histograms
    unsigned int sum = 0;
    #pragma unroll
    for (int s = 0; s < kSubHists; ++s) {
        sum += shist[s * 256 + tid];
    }
    
    // Atomic add to global output
    if (sum > 0) {
        atomicAdd(&output[tid], sum);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int grid = 256;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}