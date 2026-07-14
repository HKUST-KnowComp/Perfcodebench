#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int NUM_PARTS = 4;  // Number of sub-histograms per block to reduce bank conflicts

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[NUM_PARTS * 256];
    
    // Initialize shared memory histogram
    for (int i = threadIdx.x; i < NUM_PARTS * 256; i += BLOCK_SIZE) {
        shist[i] = 0;
    }
    __syncthreads();
    
    // Each thread uses a different sub-histogram based on threadIdx to reduce conflicts
    int part = threadIdx.x % NUM_PARTS;
    unsigned int* my_hist = shist + part * 256;
    
    // Process 4 bytes at a time using vectorized loads
    int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    int stride = BLOCK_SIZE * gridDim.x;
    
    // Handle aligned portion with uint4 loads (16 bytes at a time)
    int n16 = n / 16 * 16;
    const uchar4* input4 = reinterpret_cast<const uchar4*>(input);
    int n4 = n / 4;
    
    for (int i = tid; i < n4; i += stride) {
        uchar4 val = input4[i];
        atomicAdd(my_hist + val.x, 1U);
        atomicAdd(my_hist + val.y, 1U);
        atomicAdd(my_hist + val.z, 1U);
        atomicAdd(my_hist + val.w, 1U);
    }
    
    // Handle remaining bytes
    int base = n4 * 4;
    for (int i = base + tid; i < n; i += stride) {
        atomicAdd(my_hist + input[i], 1U);
    }
    
    __syncthreads();
    
    // Reduce sub-histograms and write to global memory
    for (int i = threadIdx.x; i < 256; i += BLOCK_SIZE) {
        unsigned int sum = 0;
        for (int p = 0; p < NUM_PARTS; p++) {
            sum += shist[p * 256 + i];
        }
        if (sum > 0) {
            atomicAdd(output + i, sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Use enough blocks to saturate the GPU but not too many to cause excessive global atomics
    int num_blocks = 256;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, n);
    }
}