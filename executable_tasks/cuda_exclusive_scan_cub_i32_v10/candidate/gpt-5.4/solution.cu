#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Warp-synchronous exclusive scan for int values.
__device__ __forceinline__ int warp_exclusive_scan(int v, unsigned mask = 0xffffffffu) {
  int x = v;
  int lane = threadIdx.x & 31;
  #pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    int y = __shfl_up_sync(mask, x, offset);
    if (lane >= offset) x += y;
  }
  return x - v;
}

// Per-block exclusive scan. Writes block sums for a second-level scan.
__global__ void block_scan_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int* __restrict__ block_sums,
                                  int n) {
  constexpr int BLOCK_THREADS = 256;
  constexpr int WARPS = BLOCK_THREADS / 32;

  __shared__ int warp_totals[WARPS];
  __shared__ int warp_prefix[WARPS];

  int gid = blockIdx.x * BLOCK_THREADS + threadIdx.x;
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;

  int v = (gid < n) ? input[gid] : 0;
  int ex = warp_exclusive_scan(v);
  int inclusive = ex + v;

  if (lane == 31) warp_totals[warp] = inclusive;
  __syncthreads();

  if (warp == 0) {
    int t = (lane < WARPS) ? warp_totals[lane] : 0;
    int p = warp_exclusive_scan(t);
    if (lane < WARPS) warp_prefix[lane] = p;
  }
  __syncthreads();

  ex += warp_prefix[warp];
  if (gid < n) output[gid] = ex;

  if (threadIdx.x == BLOCK_THREADS - 1) {
    block_sums[blockIdx.x] = ex + v;
  }
}

// Scan the block sums using a single block. This is fast because the number of
// blocks is small for benchmark-sized inputs, and avoids extra dependencies.
__global__ void scan_block_sums_kernel(const int* __restrict__ input,
                                       int* __restrict__ output,
                                       int n) {
  if (blockIdx.x != 0 || threadIdx.x != 0) return;
  int running = 0;
  for (int i = 0; i < n; ++i) {
    output[i] = running;
    running += input[i];
  }
}

__global__ void add_block_offsets_kernel(int* __restrict__ output,
                                         const int* __restrict__ block_offsets,
                                         int n) {
  constexpr int BLOCK_THREADS = 256;
  int gid = blockIdx.x * BLOCK_THREADS + threadIdx.x;
  if (gid < n) {
    output[gid] += block_offsets[blockIdx.x];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  constexpr int BLOCK_THREADS = 256;
  int num_blocks = (n + BLOCK_THREADS - 1) / BLOCK_THREADS;

  int* d_block_sums = nullptr;
  int* d_block_offsets = nullptr;

  cudaMalloc(&d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&d_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    block_scan_kernel<<<num_blocks, BLOCK_THREADS>>>(input, output, d_block_sums, n);
    if (num_blocks > 1) {
      scan_block_sums_kernel<<<1, 1>>>(d_block_sums, d_block_offsets, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK_THREADS>>>(output, d_block_offsets, n);
    }
  }

  cudaFree(d_block_sums);
  cudaFree(d_block_offsets);
}
