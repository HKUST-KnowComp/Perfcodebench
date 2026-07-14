#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[256 * 64];
    
    const int tid = threadIdx.x;
    const int lane = tid & 63;
    const int base = lane * 256;
    
    // Initialize shared memory histogram for this thread's portion
    #pragma unroll
    for (int i = 0; i < 256; i += 4) {
        shist[base + i] = 0;
        shist[base + i + 1] = 0;
        shist[base + i + 2] = 0;
        shist[base + i + 3] = 0;
    }
    __syncthreads();
    
    // Process input using vectorized loads
    const int stride = blockDim.x * gridDim.x * 16;
    int idx = (blockIdx.x * blockDim.x + tid) * 16;
    
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    const int n16 = (n / 16) * 16;
    
    while (idx < n16) {
        uint4 data = input4[idx / 16];
        
        // Process 16 bytes from uint4
        #pragma unroll
        for (int i = 0; i < 4; ++i) {
            unsigned int word;
            if (i == 0) word = data.x;
            else if (i == 1) word = data.y;
            else if (i == 2) word = data.z;
            else word = data.w;
            
            shist[base + (word & 0xFF)]++;
            shist[base + ((word >> 8) & 0xFF)]++;
            shist[base + ((word >> 16) & 0xFF)]++;
            shist[base + (word >> 24)]++;
        }
        
        idx += stride;
    }
    
    // Handle remaining elements
    int rem_idx = n16 + (blockIdx.x * blockDim.x + tid);
    while (rem_idx < n) {
        shist[base + input[rem_idx]]++;
        rem_idx += blockDim.x * gridDim.x;
    }
    
    __syncthreads();
    
    // Reduce shared histograms and write to global
    // Each thread handles 4 bins
    #pragma unroll
    for (int bin = tid; bin < 256; bin += blockDim.x) {
        unsigned int sum = 0;
        #pragma unroll
        for (int t = 0; t < 64; ++t) {
            sum += shist[t * 256 + bin];
        }
        if (sum > 0) {
            atomicAdd(output + bin, sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 64;
    const int grid = 128;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}