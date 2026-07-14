#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_warp_kernel(const unsigned char* __restrict__ input,
                                             unsigned int* __restrict__ output,
                                             int n) {
  extern __shared__ unsigned int s_hist[];  // size: 256 * warp_count

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  const int warp_count = (block_threads + 31) >> 5;  // number of warps in block

  // Zero shared histograms (per-warp privatized)
  for (int i = tid; i < 256 * warp_count; i += block_threads) {
    s_hist[i] = 0u;
  }
  __syncthreads();

  // Accumulate into per-warp shared histograms
  const int warp_id = tid >> 5;  // 0..warp_count-1
  unsigned int* warp_hist = s_hist + warp_id * 256;

  int idx = blockIdx.x * block_threads + tid;
  const int stride = block_threads * gridDim.x;

  while (idx < n) {
    unsigned char v = input[idx];
    atomicAdd(&warp_hist[(int)v], 1u);
    idx += stride;
  }

  __syncthreads();

  // Reduce per-warp histograms and update global output
  for (int bin = tid; bin < 256; bin += block_threads) {
    unsigned int sum = 0u;
#pragma unroll
    for (int w = 0; w < 32; ++w) {  // unroll upper bound; condition guards excess
      if (w < warp_count) {
        sum += s_hist[w * 256 + bin];
      }
    }
    if (sum) {
      atomicAdd(&output[bin], sum);
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  // Scale grid with problem size but cap to limit end-of-kernel atomic traffic
  int grid = (n + kBlock - 1) / kBlock;
  if (grid <= 0) grid = 1;
  if (grid > 1024) grid = 1024;

  const int warp_count = (kBlock + 31) / 32;
  const size_t shmem_bytes = static_cast<size_t>(256 * warp_count) * sizeof(unsigned int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_warp_kernel<<<grid, kBlock, shmem_bytes>>>(input, output, n);
  }
}
