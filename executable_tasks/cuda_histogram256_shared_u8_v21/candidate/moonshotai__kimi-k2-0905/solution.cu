#include "interface.h"
#include <cuda_runtime.h>
#include <cooperative_groups.h>
#include <device_launch_parameters.h>
namespace cg = cooperative_groups;

/* 256-bin histogram version exploiting shared memory warp-aggregated atomics
   and kernel fusion: histogram256_u8_v21 */
constexpr int HIST_SIZE = 256;
constexpr int WARP_SIZE = 32;

__global__ void fused_histogram_kernel(const unsigned char* in,
                                        unsigned int*       out,
                                        int                 n) {
    __shared__ unsigned int smem[HIST_SIZE + 1];   // 257 to avoid bank conflict
    cg::thread_block blk = cg::this_thread_block();
    auto tile32 = cg::tiled_partition<WARP_SIZE>(blk);
    const int lid = blk.thread_rank();
    const int tid = blockIdx.x * blk.size() + lid;

    /* 1. zero shared hist per warp in parallel (fast) */
    for (int i = lid; i < HIST_SIZE; i += blk.size()) smem[i] = 0U;
    __syncthreads();

    /* histogram phase in shared mem via warp-aggregated atomics */
    for (int idx = tid; idx < n; idx += gridDim.x * blk.size()) {
        auto lane = cg::tiled_partition<WARP_SIZE>(cg::this_thread);
        unsigned char val = in[idx];
        unsigned int active_mask = __activemask();
        if (lane.any(active_mask, lane.thread_rank() == tile32.thread_rank())) {
            unsigned int bal = __ballot_sync(0xFFFFFFFFu, val == val);
            val = __shfl_sync(0xFFFFFFFFu, val, lane.thread_rank());
            atomicAdd(&smem[val], __popc(bal));
        }
    }
    __syncthreads();

    /* write back one element per thread, coalesced */
    for (int i = lid; i < HIST_SIZE; i += blk.size()) {
        atomicAdd(&out[i], smem[i]);
    }
}

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int BLOCKS = 4096;
    constexpr int THREADS = 256;
    /* L1 prefer shared; shared occupancy already fits 257 int's per TB */
    cudaFuncSetAttribute(fused_histogram_kernel, cudaFuncAttributePreferredSharedMemoryCarveout,
                         cudaSharedmemCarveoutSharedMax);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, HIST_SIZE * sizeof(unsigned int));
        fused_histogram_kernel<<<BLOCKS, THREADS>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}