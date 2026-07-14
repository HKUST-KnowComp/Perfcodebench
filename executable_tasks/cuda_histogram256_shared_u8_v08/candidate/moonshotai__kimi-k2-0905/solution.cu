#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ out, int n) {
    __shared__ unsigned int smem[257];          // 257 to avoid bank conflicts
    const int tid = threadIdx.x;
    const int warpId = tid >> 5;
    const int lane = tid & 31;

    // zero shared memory cooperatively
    for (int i = tid; i < 257; i += blockDim.x) smem[i] = 0;
    __syncthreads();

    // grid-stride loop with 4-byte vectorized loads and 4× unroll
    const int gridStride = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;
    const uint4* vecIn = reinterpret_cast<const uint4*>(input);
    int vecCount = n >> 2;   // number of full uint4 elements

    while (idx < vecCount) {
        uint4 v = vecIn[idx];
        #pragma unroll
        for (int k = 0; k < 4; ++k) {
            unsigned char b = reinterpret_cast<unsigned char*>(&v)[k];
            smem[b]++;
        }
        idx += gridStride;
    }

    // handle tail bytes
    int tailStart = vecCount << 2;
    idx = tailStart + tid;
    while (idx < n) {
        unsigned char b = input[idx];
        smem[b]++;
        idx += blockDim.x;
    }
    __syncthreads();

    // flush shared histogram to global memory using one warp only
    if (warpId == 0) {
        for (int bin = lane; bin < 256; bin += 32) {
            unsigned int cnt = smem[bin];
            if (cnt != 0) atomicAdd(out + bin, cnt);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    constexpr int kGrid = 48;   // one block per SM on V100/A100
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}