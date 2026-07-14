#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef WARP_SIZE
#define WARP_SIZE 32
#endif

__device__ __forceinline__ int warp_inclusive_scan(int v) {
  unsigned mask = 0xffffffffu;
  #pragma unroll
  for (int offset = 1; offset < WARP_SIZE; offset <<= 1) {
    int n = __shfl_up_sync(mask, v, offset);
    if ((threadIdx.x & (WARP_SIZE - 1)) >= offset) v += n;
  }
  return v;
}

__global__ void block_exclusive_scan_kernel(const int* __restrict__ input,
                                            int* __restrict__ output,
                                            int n,
                                            int* __restrict__ block_sums) {
  __shared__ int warp_totals[WARP_SIZE];

  int gid = blockIdx.x * blockDim.x + threadIdx.x;
  int x = (gid < n) ? input[gid] : 0;

  int inclusive = warp_inclusive_scan(x);
  int lane = threadIdx.x & (WARP_SIZE - 1);
  int warp = threadIdx.x >> 5;
  int num_warps = (blockDim.x + WARP_SIZE - 1) >> 5;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = inclusive;
  __syncthreads();

  if (warp == 0) {
    int v = (lane < num_warps) ? warp_totals[lane] : 0;
    int w_inclusive = warp_inclusive_scan(v);
    if (lane < num_warps) warp_totals[lane] = w_inclusive - v;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int exclusive = block_prefix + inclusive - x;

  if (gid < n) output[gid] = exclusive;

  int valid_threads = n - blockIdx.x * blockDim.x;
  if (valid_threads > blockDim.x) valid_threads = blockDim.x;
  if (valid_threads < 0) valid_threads = 0;

  if (threadIdx.x == valid_threads - 1) {
    block_sums[blockIdx.x] = exclusive + x;
  }
}

__global__ void add_block_offsets_kernel(int* __restrict__ output,
                                         int n,
                                         const int* __restrict__ block_offsets) {
  int gid = blockIdx.x * blockDim.x + threadIdx.x;
  if (gid < n) {
    output[gid] += block_offsets[blockIdx.x];
  }
}

__global__ void single_block_exclusive_scan_kernel(const int* __restrict__ input,
                                                   int* __restrict__ output,
                                                   int n) {
  __shared__ int warp_totals[WARP_SIZE];

  int tid = threadIdx.x;
  int x = (tid < n) ? input[tid] : 0;

  int inclusive = warp_inclusive_scan(x);
  int lane = tid & (WARP_SIZE - 1);
  int warp = tid >> 5;
  int num_warps = (blockDim.x + WARP_SIZE - 1) >> 5;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = inclusive;
  __syncthreads();

  if (warp == 0) {
    int v = (lane < num_warps) ? warp_totals[lane] : 0;
    int w_inclusive = warp_inclusive_scan(v);
    if (lane < num_warps) warp_totals[lane] = w_inclusive - v;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int exclusive = block_prefix + inclusive - x;

  if (tid < n) output[tid] = exclusive;
}

inline int ceil_div(int a, int b) {
  return (a + b - 1) / b;
}

void scan_recursive(const int* input, int* output, int n) {
  if (n <= 0) return;

  constexpr int BLOCK = 256;

  if (n <= BLOCK) {
    single_block_exclusive_scan_kernel<<<1, BLOCK>>>(input, output, n);
    return;
  }

  int num_blocks = ceil_div(n, BLOCK);

  int* block_sums = nullptr;
  int* block_offsets = nullptr;
  cudaMalloc(&block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));

  block_exclusive_scan_kernel<<<num_blocks, BLOCK>>>(input, output, n, block_sums);
  scan_recursive(block_sums, block_offsets, num_blocks);
  add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, n, block_offsets);

  cudaFree(block_offsets);
  cudaFree(block_sums);
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;
  for (int iter = 0; iter < iters; ++iter) {
    scan_recursive(input, output, n);
  }
}
