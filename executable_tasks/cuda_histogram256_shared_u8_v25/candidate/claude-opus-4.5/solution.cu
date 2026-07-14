#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    // Each thread maintains a private histogram in shared memory
    // 64 threads * 256 bins = 16384 unsigned ints = 64KB shared memory
    // Use smaller privatization: 32 sub-histograms
    constexpr int NUM_PARTS = 32;
    __shared__ unsigned int shist[NUM_PARTS][256];
    
    int tid = threadIdx.x;
    int part = tid % NUM_PARTS;
    
    // Initialize shared memory histogram
    for (int i = tid; i < NUM_PARTS * 256; i += blockDim.x) {
        int p = i / 256;
        int b = i % 256;
        shist[p][b] = 0;
    }
    __syncthreads();
    
    // Process input using vectorized loads (16 bytes at a time)
    int idx = blockIdx.x * blockDim.x + tid;
    int stride = blockDim.x * gridDim.x;
    
    // Aligned vectorized processing
    int n16 = n / 16 * 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    int idx4 = idx;
    int stride4 = stride;
    
    for (int i = idx4; i < n16 / 16; i += stride4) {
        uint4 data = input4[i];
        
        // Unpack and accumulate - data.x contains 4 bytes
        unsigned int v0 = data.x;
        unsigned int v1 = data.y;
        unsigned int v2 = data.z;
        unsigned int v3 = data.w;
        
        atomicAdd(&shist[part][(v0 >> 0) & 0xFF], 1);
        atomicAdd(&shist[part][(v0 >> 8) & 0xFF], 1);
        atomicAdd(&shist[part][(v0 >> 16) & 0xFF], 1);
        atomicAdd(&shist[part][(v0 >> 24) & 0xFF], 1);
        
        atomicAdd(&shist[part][(v1 >> 0) & 0xFF], 1);
        atomicAdd(&shist[part][(v1 >> 8) & 0xFF], 1);
        atomicAdd(&shist[part][(v1 >> 16) & 0xFF], 1);
        atomicAdd(&shist[part][(v1 >> 24) & 0xFF], 1);
        
        atomicAdd(&shist[part][(v2 >> 0) & 0xFF], 1);
        atomicAdd(&shist[part][(v2 >> 8) & 0xFF], 1);
        atomicAdd(&shist[part][(v2 >> 16) & 0xFF], 1);
        atomicAdd(&shist[part][(v2 >> 24) & 0xFF], 1);
        
        atomicAdd(&shist[part][(v3 >> 0) & 0xFF], 1);
        atomicAdd(&shist[part][(v3 >> 8) & 0xFF], 1);
        atomicAdd(&shist[part][(v3 >> 16) & 0xFF], 1);
        atomicAdd(&shist[part][(v3 >> 24) & 0xFF], 1);
    }
    
    // Handle remaining elements
    for (int i = n16 + idx; i < n; i += stride) {
        atomicAdd(&shist[part][input[i]], 1);
    }
    
    __syncthreads();
    
    // Reduce shared histograms and write to global memory
    for (int bin = tid; bin < 256; bin += blockDim.x) {
        unsigned int sum = 0;
        #pragma unroll
        for (int p = 0; p < NUM_PARTS; ++p) {
            sum += shist[p][bin];
        }
        if (sum > 0) {
            atomicAdd(&output[bin], sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int numSMs;
    cudaDeviceGetAttribute(&numSMs, cudaDevAttrMultiProcessorCount, 0);
    int grid = numSMs * 2;  // 2 blocks per SM for good occupancy
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}