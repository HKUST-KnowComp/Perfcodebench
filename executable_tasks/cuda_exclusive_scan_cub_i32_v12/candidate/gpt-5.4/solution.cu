#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Warp-synchronous exclusive scan for int using shuffle intrinsics.
__device__ __forceinline__ int warp_exclusive_scan(int v, unsigned mask = 0xffffffffu) {
  int x = v;
  #pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    int y = __shfl_up_sync(mask, x, offset);
    if ((threadIdx.x & 31) >= offset) x += y;
  }
  return x - v;
}

// Single-kernel decoupled-lookback-style scan across blocks using a lightweight
// global block-state protocol. This avoids multiple kernel launches per scan.
__global__ void exclusive_scan_singlepass_kernel(
    const int* __restrict__ input,
    int* __restrict__ output,
    int n,
    int* __restrict__ block_sums,
    int* __restrict__ block_prefix,
    int* __restrict__ block_flags) {
  constexpr int BLOCK_THREADS = 256;
  constexpr int ITEMS_PER_THREAD = 8;
  constexpr int TILE_ITEMS = BLOCK_THREADS * ITEMS_PER_THREAD;
  constexpr int WARPS = BLOCK_THREADS / 32;

  __shared__ int warp_totals[WARPS];
  __shared__ int block_exclusive_prefix;

  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;
  const int block = blockIdx.x;
  const int tile_start = block * TILE_ITEMS;

  int vals[ITEMS_PER_THREAD];
  int excl[ITEMS_PER_THREAD];
  int thread_sum = 0;

  #pragma unroll
  for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
    int idx = tile_start + tid + i * BLOCK_THREADS;
    int v = 0;
    if (idx < n) v = input[idx];
    vals[i] = v;
    excl[i] = thread_sum;
    thread_sum += v;
  }

  int warp_excl = warp_exclusive_scan(thread_sum);
  int warp_total = warp_excl + thread_sum;
  if (lane == 31) warp_totals[warp] = warp_total;
  __syncthreads();

  if (warp == 0) {
    int wv = (lane < WARPS) ? warp_totals[lane] : 0;
    int wex = warp_exclusive_scan(wv);
    if (lane < WARPS) warp_totals[lane] = wex;
  }
  __syncthreads();

  int thread_prefix = warp_totals[warp] + warp_excl;
  int tile_sum = warp_totals[WARPS - 1] + warp_total;

  if (tid == 0) {
    block_sums[block] = tile_sum;
    __threadfence();
    block_flags[block] = 1;  // local sum ready

    if (block == 0) {
      block_prefix[0] = 0;
      __threadfence();
      block_flags[0] = 2;  // prefix ready
    } else {
      int prefix = 0;
      int prev = block - 1;
      while (true) {
        int flag = atomicAdd(&block_flags[prev], 0);
        if (flag >= 2) {
          prefix = block_prefix[prev] + block_sums[prev];
          break;
        }
      }
      block_prefix[block] = prefix;
      __threadfence();
      block_flags[block] = 2;  // prefix ready
    }
    block_exclusive_prefix = block_prefix[block];
  }
  __syncthreads();

  int base = block_exclusive_prefix + thread_prefix;
  #pragma unroll
  for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
    int idx = tile_start + tid + i * BLOCK_THREADS;
    if (idx < n) output[idx] = base + excl[i];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;
  if (n <= 0) return;

  constexpr int BLOCK_THREADS = 256;
  constexpr int ITEMS_PER_THREAD = 8;
  constexpr int TILE_ITEMS = BLOCK_THREADS * ITEMS_PER_THREAD;

  const int num_blocks = (n + TILE_ITEMS - 1) / TILE_ITEMS;

  int* block_sums = nullptr;
  int* block_prefix = nullptr;
  int* block_flags = nullptr;

  cudaMalloc(&block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&block_prefix, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&block_flags, static_cast<size_t>(num_blocks) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(block_flags, 0, static_cast<size_t>(num_blocks) * sizeof(int));
    exclusive_scan_singlepass_kernel<<<num_blocks, BLOCK_THREADS>>>(
        input, output, n, block_sums, block_prefix, block_flags);
  }

  cudaFree(block_flags);
  cudaFree(block_prefix);
  cudaFree(block_sums);
}
