#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_u8_kernel(const unsigned char* __restrict__ input,
                                              unsigned int* __restrict__      output,
                                              int n)
{
    // 256 bins * 4 bytes = 1024 bytes per block (fits in 48 KB shared memory)
    __shared__ unsigned int sh_bins[256];

    // Initialize shared bins
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
        sh_bins[i] = 0;
    __syncthreads();

    // Grid-stride loop over input data
    const int tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = gridDim.x * blockDim.x;

    for (int idx = tid; idx < n; idx += stride) {
        unsigned char val = input[idx];

        // Warp-aggregated increment: each warp computes its own partial histogram
        unsigned int mask = __ballot_sync(0xFFFFFFFF, val == val); // full warp mask
        unsigned int lane_mask = 1U << (threadIdx.x & 31);

        // Use warp shuffle to aggregate counts within warp
        for (int offset = 16; offset > 0; offset /= 2) {
            unsigned char peer_val = __shfl_xor_sync(0xFFFFFFFF, val, offset);
            unsigned int peer_mask = __ballot_sync(0xFFFFFFFF, peer_val == val);
            if (val == peer_val)
                lane_mask |= peer_mask;
        }

        // Thread holding the lowest lane of each unique value adds the count
        if ((lane_mask & ((1U << (threadIdx.x & 31)) - 1)) == 0) {
            unsigned int count = __popc(lane_mask);
            atomicAdd(&sh_bins[val], count);
        }
    }
    __syncthreads();

    // Write per-block histogram to global memory
    for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
        atomicAdd(&output[bin], sh_bins[bin]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters)
{
    constexpr int kBlock = 128;            // 4 warps per block
    const int kGrid = 960;                 // tuned for most GPUs

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram256_shared_u8_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}
