#include "interface.h"

#include <cuda_runtime.h>

namespace {

template <int BLOCK_SIZE>
__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
  constexpr int NUM_WARPS = BLOCK_SIZE / 32;

  extern __shared__ unsigned int s_hist[];

  // Zero shared memory
  #pragma unroll
  for (int i = threadIdx.x; i < NUM_WARPS * 256; i += BLOCK_SIZE) {
    s_hist[i] = 0;
  }
  __syncthreads();

  const int warpId = threadIdx.x >> 5;
  unsigned int* warp_hist = s_hist + (warpId << 8);

  const uint4* input4 = reinterpret_cast<const uint4*>(input);
  const unsigned int n4 = static_cast<unsigned int>(n) >> 4;

  const int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x;
  const unsigned int gridStride = BLOCK_SIZE * gridDim.x;

  for (unsigned int i = idx; i < n4; i += gridStride) {
    uint4 v = __ldg(input4 + i);
    unsigned int vals[4] = {v.x, v.y, v.z, v.w};
    #pragma unroll
    for (int j = 0; j < 4; ++j) {
      unsigned int val = vals[j];
      atomicAdd(warp_hist + (val & 0xFFU), 1U);
      atomicAdd(warp_hist + ((val >> 8) & 0xFFU), 1U);
      atomicAdd(warp_hist + ((val >> 16) & 0xFFU), 1U);
      atomicAdd(warp_hist + ((val >> 24) & 0xFFU), 1U);
    }
  }

  // Process remaining elements
  const int tail_start = static_cast<int>(n4 << 4);
  for (int i = tail_start + idx; i < n; i += gridStride) {
    atomicAdd(warp_hist + input[i], 1U);
  }

  __syncthreads();

  // Reduce per-warp histograms to global memory
  #pragma unroll
  for (int bin = threadIdx.x; bin < 256; bin += BLOCK_SIZE) {
    unsigned int sum = 0;
    #pragma unroll
    for (int w = 0; w < NUM_WARPS; ++w) {
      sum += s_hist[(w << 8) + bin];
    }
    atomicAdd(output + bin, sum);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  int grid = (n + kBlock - 1) / kBlock;
  if (grid > 2048) grid = 2048;
  if (grid < 1) grid = 1;
  const size_t smem = (kBlock / 32) * 256 * sizeof(unsigned int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram256_shared_kernel<kBlock><<<grid, kBlock, smem>>>(input, output, n);
  }
}
