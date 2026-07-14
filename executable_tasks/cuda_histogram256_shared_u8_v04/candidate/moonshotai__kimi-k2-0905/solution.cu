#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int smem[256];

    // one thread per bin to init shared mem
    if (threadIdx.x < 256)
        smem[threadIdx.x] = 0;
    __syncthreads();

    // grid-stride loop over input
    for (int i = blockIdx.x * blockDim.x + threadIdx.x;
         i < n;
         i += blockDim.x * gridDim.x) {
        unsigned char val = input[i];
        // warp-aggregated increment avoids intra-warp conflicts
        unsigned int mask = __activemask();
        unsigned int leader = __ffs(mask) - 1;
        unsigned int lane = threadIdx.x & 31;
        if (lane == leader) {
            smem[val] += __popc(mask);
        }
    }
    __syncthreads();

    // reduce shared histogram to global with one atomic per bin
    if (threadIdx.x < 256) {
        atomicAdd(output + threadIdx.x, smem[threadIdx.x]);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int block = 256;
    const int grid = (n + block - 1) / block;  // cover all elements

    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    for (int iter = 0; iter < iters; ++iter) {
        histogram_shared_kernel<<<grid, block>>>(input, output, n);
    }
}