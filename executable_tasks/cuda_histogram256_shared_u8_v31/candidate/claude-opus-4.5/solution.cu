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
    int part = tid & (NUM_PARTS - 1);

    #pragma unroll
    for (int i = tid; i < 256 * NUM_PARTS; i += BLOCK_SIZE) {
        smem[i] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * BLOCK_SIZE + tid;
    int stride = BLOCK_SIZE * gridDim.x;

    int n16 = (n / 16) * 16;

    for (int i = idx * 16; i < n16; i += stride * 16) {
        uint4 v = *reinterpret_cast<const uint4*>(input + i);

        unsigned char b0  = (v.x) & 0xFF;
        unsigned char b1  = (v.x >> 8) & 0xFF;
        unsigned char b2  = (v.x >> 16) & 0xFF;
        unsigned char b3  = (v.x >> 24) & 0xFF;
        unsigned char b4  = (v.y) & 0xFF;
        unsigned char b5  = (v.y >> 8) & 0xFF;
        unsigned char b6  = (v.y >> 16) & 0xFF;
        unsigned char b7  = (v.y >> 24) & 0xFF;
        unsigned char b8  = (v.z) & 0xFF;
        unsigned char b9  = (v.z >> 8) & 0xFF;
        unsigned char b10 = (v.z >> 16) & 0xFF;
        unsigned char b11 = (v.z >> 24) & 0xFF;
        unsigned char b12 = (v.w) & 0xFF;
        unsigned char b13 = (v.w >> 8) & 0xFF;
        unsigned char b14 = (v.w >> 16) & 0xFF;
        unsigned char b15 = (v.w >> 24) & 0xFF;

        atomicAdd(&smem[b0  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b1  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b2  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b3  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b4  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b5  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b6  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b7  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b8  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b9  * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b10 * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b11 * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b12 * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b13 * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b14 * NUM_PARTS + part], 1U);
        atomicAdd(&smem[b15 * NUM_PARTS + part], 1U);
    }

    int tail_start = n16 + idx;
    for (int i = tail_start; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&smem[val * NUM_PARTS + part], 1U);
    }

    __syncthreads();

    #pragma unroll
    for (int bin = tid; bin < 256; bin += BLOCK_SIZE) {
        unsigned int sum = 0;
        #pragma unroll
        for (int p = 0; p < NUM_PARTS; ++p) {
            sum += smem[bin * NUM_PARTS + p];
        }
        if (sum > 0) {
            atomicAdd(&output[bin], sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    int grid = 256;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, BLOCK_SIZE>>>(input, output, n);
    }
}