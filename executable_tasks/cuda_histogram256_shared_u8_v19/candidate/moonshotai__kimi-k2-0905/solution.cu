#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
    __shared__ unsigned int smem[256 + 32]; // 256 bins + padding to avoid bank conflicts

    // initialise shared histogram
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
        smem[i] = 0;
    __syncthreads();

    // grid-stride loop over input data
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += gridDim.x * blockDim.x) {
        unsigned char val = input[idx];
        // warp-aggregated increment: ballot + popc
        unsigned mask = __ballot_sync(0xFFFFFFFF, val == (threadIdx.x & 0xFF));
        if ((threadIdx.x & 0xFF) == val) {
            unsigned count = __popc(mask);
            if (count > 0)
                atomicAdd(&smem[val], count);
        }
    }
    __syncthreads();

    // flush shared histogram to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
        atomicAdd(&output[i], smem[i]);
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int block = 256;
    const int grid = 1024;

    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    for (int iter = 0; iter < iters; ++iter) {
        histogram256_shared_kernel<<<grid, block>>>(input, output, n);
    }
}