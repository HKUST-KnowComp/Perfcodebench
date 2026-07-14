#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Kernel that builds per-warp histograms in shared memory to reduce global atomics.
__global__ void histogram_shared_warp_kernel(const unsigned char* __restrict__ input,
                                             unsigned int* __restrict__ output,
                                             int n) {
  extern __shared__ unsigned int s_hist[];  // size: warps * 256

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  const int warps = block_threads / 32;
  const int warp_id = tid / 32;

  // Zero initialize shared histograms
  for (int i = tid; i < warps * 256; i += block_threads) {
    s_hist[i] = 0u;
  }
  __syncthreads();

  unsigned int* my_hist = s_hist + warp_id * 256;

  // Grid-stride loop over input elements
  const int total_threads = gridDim.x * block_threads;
  for (int idx = blockIdx.x * block_threads + tid; idx < n; idx += total_threads) {
    unsigned char v = input[idx];
    // Shared memory atomic add: faster than global and reduces contention
    atomicAdd(&my_hist[v], 1u);
  }

  __syncthreads();

  // Reduce per-warp histograms to global output: 256 bins per block
  if (tid < 256) {
    unsigned int sum = 0u;
    for (int w = 0; w < warps; ++w) {
      sum += s_hist[w * 256 + tid];
    }
    if (sum) {
      atomicAdd(output + tid, sum);
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  constexpr int kBlock = 256;  // 8 warps; 8*256*4 = 8 KB shared memory

  // Choose grid size: cap to 1024 and not exceed work size
  int maxBlocks = 1024;
  int grid = (n + kBlock - 1) / kBlock;  // enough to cover data once
  if (grid <= 0) grid = 1;
  if (grid > maxBlocks) grid = maxBlocks;

  size_t smem_size = (kBlock / 32) * 256 * sizeof(unsigned int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_warp_kernel<<<grid, kBlock, smem_size>>>(input, output, n);
  }
}
