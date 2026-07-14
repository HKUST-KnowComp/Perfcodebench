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

__global__ void block_scan_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int n,
                                  int* __restrict__ block_sums) {
  __shared__ int warp_totals[WARP_SIZE];

  int gid = blockIdx.x * blockDim.x + threadIdx.x;
  int x = (gid < n) ? input[gid] : 0;

  int inclusive = warp_inclusive_scan(x);
  int lane = threadIdx.x & (WARP_SIZE - 1);
  int warp = threadIdx.x >> 5;
  int warp_count = blockDim.x >> 5;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = inclusive;
  __syncthreads();

  if (warp == 0) {
    int v = (lane < warp_count) ? warp_totals[lane] : 0;
    int inc = warp_inclusive_scan(v);
    warp_totals[lane] = inc - v;
  }
  __syncthreads();

  int exclusive = inclusive - x + warp_totals[warp];
  if (gid < n) output[gid] = exclusive;

  if (threadIdx.x == blockDim.x - 1) {
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

__global__ void scan_block_sums_single_block(const int* __restrict__ input,
                                             int* __restrict__ output,
                                             int n) {
  __shared__ int temp[1024];
  int tid = threadIdx.x;
  int x = (tid < n) ? input[tid] : 0;
  temp[tid] = x;
  __syncthreads();

  for (int offset = 1; offset < blockDim.x; offset <<= 1) {
    int val = 0;
    if (tid >= offset) val = temp[tid - offset];
    __syncthreads();
    if (tid < n) temp[tid] += val;
    __syncthreads();
  }

  if (tid < n) output[tid] = temp[tid] - x;
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  constexpr int BLOCK = 256;
  int num_blocks = (n + BLOCK - 1) / BLOCK;

  int* d_block_sums = nullptr;
  int* d_block_offsets = nullptr;

  cudaMalloc(&d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&d_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));

  if (num_blocks == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      block_scan_kernel<<<1, BLOCK>>>(input, output, n, d_block_sums);
    }
    cudaFree(d_block_sums);
    cudaFree(d_block_offsets);
    return;
  }

  int scan_threads = 1;
  while (scan_threads < num_blocks) scan_threads <<= 1;
  if (scan_threads > 1024) scan_threads = 1024;

  for (int iter = 0; iter < iters; ++iter) {
    block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, n, d_block_sums);
    scan_block_sums_single_block<<<1, scan_threads>>>(d_block_sums, d_block_offsets, num_blocks);
    add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, n, d_block_offsets);
  }

  cudaFree(d_block_sums);
  cudaFree(d_block_offsets);
}
