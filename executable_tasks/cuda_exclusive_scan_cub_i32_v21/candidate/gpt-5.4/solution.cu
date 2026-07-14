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

__global__ void scan_block_sums_kernel(const int* __restrict__ input,
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

  if (lane == WARP_SIZE - 1) {
    warp_totals[warp] = inclusive;
  }
  __syncthreads();

  if (warp == 0) {
    int v = (lane < num_warps) ? warp_totals[lane] : 0;
    int inc = warp_inclusive_scan(v);
    warp_totals[lane] = inc - v;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int exclusive = block_prefix + inclusive - x;

  if (gid < n) {
    output[gid] = exclusive;
  }

  int valid_in_block = n - blockIdx.x * blockDim.x;
  if (valid_in_block > blockDim.x) valid_in_block = blockDim.x;
  if (valid_in_block < 0) valid_in_block = 0;

  if (threadIdx.x == valid_in_block - 1) {
    block_sums[blockIdx.x] = exclusive + x;
  }
}

__global__ void scan_sums_kernel(int* data, int n) {
  __shared__ int warp_totals[WARP_SIZE];

  int tid = threadIdx.x;
  int x = (tid < n) ? data[tid] : 0;

  int inclusive = warp_inclusive_scan(x);

  int lane = tid & (WARP_SIZE - 1);
  int warp = tid >> 5;
  int num_warps = (blockDim.x + WARP_SIZE - 1) >> 5;

  if (lane == WARP_SIZE - 1) {
    warp_totals[warp] = inclusive;
  }
  __syncthreads();

  if (warp == 0) {
    int v = (lane < num_warps) ? warp_totals[lane] : 0;
    int inc = warp_inclusive_scan(v);
    warp_totals[lane] = inc - v;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int exclusive = block_prefix + inclusive - x;

  if (tid < n) {
    data[tid] = exclusive;
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

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  constexpr int BLOCK = 256;
  int num_blocks = (n + BLOCK - 1) / BLOCK;

  int* block_sums = nullptr;
  if (num_blocks > 1) {
    cudaMalloc(&block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (num_blocks == 1) {
      scan_block_sums_kernel<<<1, BLOCK>>>(input, output, output, n);
    } else {
      scan_block_sums_kernel<<<num_blocks, BLOCK>>>(input, output, block_sums, n);

      int scan_threads = 1;
      while (scan_threads < num_blocks) scan_threads <<= 1;
      if (scan_threads < WARP_SIZE) scan_threads = WARP_SIZE;
      if (scan_threads > 1024) scan_threads = 1024;

      scan_sums_kernel<<<1, scan_threads>>>(block_sums, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, block_sums, n);
    }
  }

  if (block_sums) cudaFree(block_sums);
}
