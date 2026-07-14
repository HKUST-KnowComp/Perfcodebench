#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[256];
    
    // Initialize shared memory histogram
    int tid = threadIdx.x;
    if (tid < 256) {
        shist[tid] = 0;
    }
    __syncthreads();
    
    // Process input using vectorized loads (16 bytes at a time)
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    // Process 16 bytes per thread per iteration using uint4
    int n16 = n / 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);
    
    for (int i = idx; i < n16; i += stride) {
        uint4 data = input4[i];
        
        // Extract and count all 16 bytes
        atomicAdd(&shist[(data.x) & 0xFF], 1);
        atomicAdd(&shist[(data.x >> 8) & 0xFF], 1);
        atomicAdd(&shist[(data.x >> 16) & 0xFF], 1);
        atomicAdd(&shist[(data.x >> 24) & 0xFF], 1);
        
        atomicAdd(&shist[(data.y) & 0xFF], 1);
        atomicAdd(&shist[(data.y >> 8) & 0xFF], 1);
        atomicAdd(&shist[(data.y >> 16) & 0xFF], 1);
        atomicAdd(&shist[(data.y >> 24) & 0xFF], 1);
        
        atomicAdd(&shist[(data.z) & 0xFF], 1);
        atomicAdd(&shist[(data.z >> 8) & 0xFF], 1);
        atomicAdd(&shist[(data.z >> 16) & 0xFF], 1);
        atomicAdd(&shist[(data.z >> 24) & 0xFF], 1);
        
        atomicAdd(&shist[(data.w) & 0xFF], 1);
        atomicAdd(&shist[(data.w >> 8) & 0xFF], 1);
        atomicAdd(&shist[(data.w >> 16) & 0xFF], 1);
        atomicAdd(&shist[(data.w >> 24) & 0xFF], 1);
    }
    
    // Handle remaining bytes
    int base = n16 * 16;
    for (int i = base + idx; i < n; i += stride) {
        atomicAdd(&shist[input[i]], 1);
    }
    
    __syncthreads();
    
    // Merge shared histogram to global using all threads
    if (tid < 256) {
        unsigned int val = shist[tid];
        if (val > 0) {
            atomicAdd(&output[tid], val);
        }
    }
}

__global__ void memset_kernel(unsigned int* output) {
    int tid = threadIdx.x;
    if (tid < 256) {
        output[tid] = 0;
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int numSMs;
    cudaDeviceGetAttribute(&numSMs, cudaDevAttrMultiProcessorCount, 0);
    int grid = numSMs * 8;  // Multiple blocks per SM for occupancy
    
    for (int iter = 0; iter < iters; ++iter) {
        memset_kernel<<<1, 256>>>(output);
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}