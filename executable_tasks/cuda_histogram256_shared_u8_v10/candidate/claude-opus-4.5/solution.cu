#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int NUM_PARTS = 4;

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[NUM_PARTS * 256];

    int tid = threadIdx.x;
    
    #pragma unroll
    for (int i = 0; i < NUM_PARTS; ++i) {
        shist[i * 256 + tid] = 0;
    }
    __syncthreads();

    int part = tid % NUM_PARTS;
    unsigned int* my_hist = shist + part * 256;

    int base = blockIdx.x * blockDim.x * 16 + tid * 16;
    int stride = gridDim.x * blockDim.x * 16;

    for (int idx = base; idx + 15 < n; idx += stride) {
        uint4 v = *reinterpret_cast<const uint4*>(input + idx);
        
        unsigned int w0 = v.x;
        unsigned int w1 = v.y;
        unsigned int w2 = v.z;
        unsigned int w3 = v.w;

        atomicAdd(my_hist + (w0 & 0xFF), 1U);
        atomicAdd(my_hist + ((w0 >> 8) & 0xFF), 1U);
        atomicAdd(my_hist + ((w0 >> 16) & 0xFF), 1U);
        atomicAdd(my_hist + ((w0 >> 24) & 0xFF), 1U);

        atomicAdd(my_hist + (w1 & 0xFF), 1U);
        atomicAdd(my_hist + ((w1 >> 8) & 0xFF), 1U);
        atomicAdd(my_hist + ((w1 >> 16) & 0xFF), 1U);
        atomicAdd(my_hist + ((w1 >> 24) & 0xFF), 1U);

        atomicAdd(my_hist + (w2 & 0xFF), 1U);
        atomicAdd(my_hist + ((w2 >> 8) & 0xFF), 1U);
        atomicAdd(my_hist + ((w2 >> 16) & 0xFF), 1U);
        atomicAdd(my_hist + ((w2 >> 24) & 0xFF), 1U);

        atomicAdd(my_hist + (w3 & 0xFF), 1U);
        atomicAdd(my_hist + ((w3 >> 8) & 0xFF), 1U);
        atomicAdd(my_hist + ((w3 >> 16) & 0xFF), 1U);
        atomicAdd(my_hist + ((w3 >> 24) & 0xFF), 1U);
    }

    int tail_base = (n / (gridDim.x * blockDim.x * 16)) * (gridDim.x * blockDim.x * 16);
    int tail_start = tail_base + blockIdx.x * blockDim.x + tid;
    for (int idx = tail_start; idx < n; idx += gridDim.x * blockDim.x) {
        atomicAdd(my_hist + input[idx], 1U);
    }

    __syncthreads();

    #pragma unroll
    for (int bin = tid; bin < 256; bin += BLOCK_SIZE) {
        unsigned int sum = 0;
        #pragma unroll
        for (int p = 0; p < NUM_PARTS; ++p) {
            sum += shist[p * 256 + bin];
        }
        if (sum > 0) {
            atomicAdd(output + bin, sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    int grid = 256;
    if (n > 1 << 24) grid = 512;
    if (n > 1 << 26) grid = 1024;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, BLOCK_SIZE>>>(input, output, n);
    }
}