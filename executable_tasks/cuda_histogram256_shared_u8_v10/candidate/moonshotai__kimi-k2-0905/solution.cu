#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram256_kernel(const unsigned char* __restrict__ input,
                                    unsigned int* __restrict__ output,
                                    int n) {
  // 256 histogram bins, 32 warps per block (1024 threads)
  static_assert(blockDim.x == 1024, "Kernel expects 1024 threads per block");

  __shared__ unsigned int smem[256];

  // Warp-private counters (256 bytes) in registers
  unsigned int warpHist[8];  // 8*4 = 32 bytes, 8 lanes cooperate per 32 bins
#pragma unroll
  for (int i = 0; i < 8; ++i) warpHist[i] = 0;

  const int tid = threadIdx.x;
  const int laneId = tid & 31;
  const int warpId = tid >> 5;

  // Grid-stride loop
  for (int idx = blockIdx.x * blockDim.x + tid; idx < n;
       idx += blockDim.x * gridDim.x) {
    unsigned char val = input[idx];
    int sub = val >> 2;  // 0..63 maps to one of 64 counters
    unsigned mask = __ballot_sync(0xFFFFFFFF, (val & 3) == 0);
    if ((val & 3) == 0) warpHist[sub] += __popc(mask);
    mask = __ballot_sync(0xFFFFFFFF, (val & 3) == 1);
    if ((val & 3) == 1) warpHist[sub] += __popc(mask);
    mask = __ballot_sync(0xFFFFFFFF, (val & 3) == 2);
    if ((val & 3) == 2) warpHist[sub] += __popc(mask);
    mask = __ballot_sync(0xFFFFFFFF, (val & 3) == 3);
    if ((val & 3) == 3) warpHist[sub] += __popc(mask);
  }

  // Reduce warp counters into shared memory (per-block histogram)
#pragma unroll
  for (int i = 0; i < 8; ++i) {
    int bin = (i << 2) + (laneId & 3);
    if (bin < 32) {
      unsigned int cnt = warpHist[i];
      // Use warp-level reduction
      cnt += __shfl_down_sync(0xFFFFFFFF, cnt, 16);
      cnt += __shfl_down_sync(0xFFFFFFFF, cnt, 8);
      cnt += __shfl_down_sync(0xFFFFFFFF, cnt, 4);
      cnt += __shfl_down_sync(0xFFFFFFFF, cnt, 2);
      cnt += __shfl_down_sync(0xFFFFFFFF, cnt, 1);
      if ((laneId & 31) == 0) atomicAdd(&smem[bin + (warpId << 5)], cnt);
    }
  }
  __syncthreads();

  // Final warp (warp 0) writes block histogram to global memory
  if (warpId == 0) {
    for (int bin = laneId; bin < 256; bin += 32) {
      unsigned int cnt = smem[bin];
      // Warp-level reduction across blocks
      for (int offset = 16; offset > 0; offset >>= 1) {
        cnt += __shfl_down_sync(0xFFFFFFFF, cnt, offset);
      }
      if ((laneId & 31) == 0) {
        if (blockIdx.x == 0) output[bin] = cnt;
        else atomicAdd(&output[bin], cnt);
      }
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n,
                     int iters) {
  constexpr int kThreadsPerBlock = 1024;
  const int kBlocks = (n + kThreadsPerBlock - 1) / kThreadsPerBlock;

  cudaMemset(output, 0, 256 * sizeof(unsigned int));
  for (int iter = 0; iter < iters; ++iter) {
    histogram256_kernel<<<kBlocks, kThreadsPerBlock>>>(input, output, n);
  }
}