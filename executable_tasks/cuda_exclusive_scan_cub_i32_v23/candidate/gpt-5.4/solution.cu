#include "interface.h"

#include <cuda_runtime.h>
#include <cstdint>

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

__global__ void block_scan_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int* __restrict__ block_sums,
                                  int n) {
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
    int inc = warp_inclusive_scan(v);
    warp_totals[lane] = inc - v;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int exclusive = block_prefix + inclusive - x;

  if (gid < n) output[gid] = exclusive;

  int valid = n - blockIdx.x * blockDim.x;
  if (valid > blockDim.x) valid = blockDim.x;
  if (valid < 0) valid = 0;

  if (threadIdx.x == valid - 1) {
    block_sums[blockIdx.x] = inclusive + block_prefix;
  }
}

__global__ void add_block_offsets_kernel(int* __restrict__ output,
                                         const int* __restrict__ block_offsets,
                                         int n) {
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
    int inc = warp_inclusive_scan(v);
    warp_totals[lane] = inc - v;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int exclusive = block_prefix + inclusive - x;
  if (tid < n) output[tid] = exclusive;
}

static inline int ceil_div(int a, int b) {
  return (a + b - 1) / b;
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0 || n <= 0) return;

  constexpr int BLOCK = 256;

  if (n <= BLOCK) {
    for (int iter = 0; iter < iters; ++iter) {
      single_block_exclusive_scan_kernel<<<1, BLOCK>>>(input, output, n);
    }
    return;
  }

  int num_blocks = ceil_div(n, BLOCK);

  int* block_sums = nullptr;
  int* block_offsets = nullptr;
  cudaMalloc(&block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));

  if (num_blocks <= BLOCK) {
    for (int iter = 0; iter < iters; ++iter) {
      block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, block_sums, n);
      single_block_exclusive_scan_kernel<<<1, BLOCK>>>(block_sums, block_offsets, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, block_offsets, n);
    }
  } else {
    int* level1_sums = nullptr;
    int* level1_offsets = nullptr;
    int level1_blocks = ceil_div(num_blocks, BLOCK);
    cudaMalloc(&level1_sums, static_cast<size_t>(level1_blocks) * sizeof(int));
    cudaMalloc(&level1_offsets, static_cast<size_t>(level1_blocks) * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
      block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, block_sums, n);
      block_scan_kernel<<<level1_blocks, BLOCK>>>(block_sums, block_offsets, level1_sums, num_blocks);
      single_block_exclusive_scan_kernel<<<1, BLOCK>>>(level1_sums, level1_offsets, level1_blocks);
      add_block_offsets_kernel<<<level1_blocks, BLOCK>>>(block_offsets, level1_offsets, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, block_offsets, n);
    }

    cudaFree(level1_sums);
    cudaFree(level1_offsets);
  }

  cudaFree(block_sums);
  cudaFree(block_offsets);
}
