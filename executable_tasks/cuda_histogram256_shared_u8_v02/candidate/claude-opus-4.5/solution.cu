#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int NUM_PARTS = 4;

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int smem[256 * NUM_PARTS];

    int tid = threadIdx.x;
    
    #pragma unroll
    for (int i = 0; i < NUM_PARTS; ++i) {
        smem[tid + i * 256] = 0;
    }
    __syncthreads();

    int part = tid % NUM_PARTS;
    unsigned int* my_hist = smem + part * 256;

    int idx = (blockIdx.x * BLOCK_SIZE + tid) * 16;
    int stride = gridDim.x * BLOCK_SIZE * 16;
    int n16 = (n / 16) * 16;

    while (idx < n16) {
        uint4 data = *reinterpret_cast<const uint4*>(input + idx);
        
        unsigned char bytes[16];
        *reinterpret_cast<uint4*>(bytes) = data;
        
        #pragma unroll
        for (int i = 0; i < 16; ++i) {
            atomicAdd(my_hist + bytes[i], 1U);
        }
        
        idx += stride;
    }

    int scalar_start = n16 + blockIdx.x * BLOCK_SIZE + tid;
    int scalar_stride = gridDim.x * BLOCK_SIZE;
    for (int i = scalar_start; i < n; i += scalar_stride) {
        atomicAdd(my_hist + input[i], 1U);
    }

    __syncthreads();

    #pragma unroll
    for (int i = tid; i < 256; i += BLOCK_SIZE) {
        unsigned int sum = 0;
        #pragma unroll
        for (int p = 0; p < NUM_PARTS; ++p) {
            sum += smem[i + p * 256];
        }
        if (sum > 0) {
            atomicAdd(output + i, sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    int grid = 256;
    if (n > 1 << 24) grid = 512;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, BLOCK_SIZE>>>(input, output, n);
    }
}