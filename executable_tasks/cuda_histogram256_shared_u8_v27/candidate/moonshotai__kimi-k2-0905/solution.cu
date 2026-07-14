#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int hist[256];

    // Phase 1: zero shared histogram
    for (int i = threadIdx.x; i < 256; i += blockDim.x) hist[i] = 0;
    __syncthreads();

    // Phase 2: accumulate into shared memory (no atomics)
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x;
         idx < n;
         idx += blockDim.x * gridDim.x) {
        unsigned char val = input[idx];
        __threadfence_block();        // ensure ordering inside block
        hist[val]++;
    }
    __syncthreads();

    // Phase 3: one warp reduces shared hist into global memory
    if (threadIdx.x < 32) {           // single warp
        for (int bin = threadIdx.x; bin < 256; bin += 32) {
            unsigned int cnt = hist[bin];
            if (cnt != 0) atomicAdd(output + bin, cnt);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int block = 256;
    const int grid  = (n + block - 1) / block;   // enough blocks to cover input

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, block, 256 * sizeof(unsigned int)>>>(input, output, n);
    }
}