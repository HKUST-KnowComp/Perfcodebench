#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n)
{
    __shared__ unsigned int hist[256 + 32];   // 256 bins + 32 padding to avoid bank conflicts
    const int tid = threadIdx.x;
    const int gtid = blockIdx.x * blockDim.x + tid;

    // zero shared histogram
    for (int i = tid; i < 256; i += blockDim.x) hist[i] = 0;
    __syncthreads();

    // phase 1: private histogram in shared memory
    for (int idx = gtid; idx < n; idx += gridDim.x * blockDim.x) {
        unsigned char val = input[idx];
        atomicAdd(&hist[val], 1U);
    }
    __syncthreads();

    // phase 2: coalesced flush to global
    for (int i = tid; i < 256; i += blockDim.x) {
        unsigned int cnt = hist[i];
        if (cnt) atomicAdd(output + i, cnt);
    }
}
} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int kGrid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}