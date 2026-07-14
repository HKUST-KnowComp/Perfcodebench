#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int smem[256];

    // Each thread zeroes a subset of the shared histogram
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        smem[i] = 0;
    }
    __syncthreads();

    // Grid-stride loop over input data
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x;
         idx < n;
         idx += blockDim.x * gridDim.x) {
        unsigned char val = input[idx];
        // Race-free update within a block
        smem[val]++;
    }
    __syncthreads();

    // Warp-level reduction to global memory
    const int warpId = threadIdx.x >> 5;
    const int laneId = threadIdx.x & 31;
    const int warpsPerBlock = blockDim.x >> 5;

    for (int bin = warpId; bin < 256; bin += warpsPerBlock) {
        unsigned int cnt = 0;
        // Reduce within warp
        for (int t = laneId; t < 256; t += 32) {
            if (t == bin) cnt += smem[t];
        }
        // Warp shuffle reduction
        for (int offset = 16; offset >= 1; offset >>= 1) {
            cnt += __shfl_down_sync(0xFFFFFFFF, cnt, offset);
        }
        // Thread 0 of warp writes to global
        if (laneId == 0) {
            atomicAdd(output + bin, cnt);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int kGrid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}