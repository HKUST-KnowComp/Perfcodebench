#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n)
{
    __shared__ unsigned int smem[256];

    // One thread per bin to initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256)
        smem[tid] = 0;
    __syncthreads();

    // Grid-stride loop over input
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        unsigned char val = input[idx];
        atomicAdd(&smem[val], 1U);
    }
    __syncthreads();

    // Warp-level reduction to global memory
    if (tid < 256) {
        // Each warp handles one bin; here only one warp is used (tid < 32)
        unsigned int sum = smem[tid];
        for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
            sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
        }
        if (tid % warpSize == 0) {
            atomicAdd(output + tid, sum);
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