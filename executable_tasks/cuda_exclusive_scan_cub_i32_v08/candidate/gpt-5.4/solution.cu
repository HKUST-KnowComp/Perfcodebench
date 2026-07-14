#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef WARP_SIZE
#define WARP_SIZE 32
#endif

__device__ __forceinline__ int warp_exclusive_scan(int v, unsigned mask = 0xffffffffu) {
  int x = v;
#pragma unroll
  for (int offset = 1; offset < WARP_SIZE; offset <<= 1) {
    int y = __shfl_up_sync(mask, x, offset);
    if ((threadIdx.x & (WARP_SIZE - 1)) >= offset) x += y;
  }
  return x - v;
}

__global__ void block_scan_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int* __restrict__ block_sums,
                                  int n) {
  __shared__ int warp_totals[WARP_SIZE];

  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  const int lane = threadIdx.x & (WARP_SIZE - 1);
  const int warp = threadIdx.x >> 5;
  const int num_warps = blockDim.x >> 5;

  int v = (gid < n) ? input[gid] : 0;
  int ex = warp_exclusive_scan(v);
  int inclusive = ex + v;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = inclusive;
  __syncthreads();

  if (warp == 0) {
    int t = (lane < num_warps) ? warp_totals[lane] : 0;
    int ex_warp = warp_exclusive_scan(t);
    warp_totals[lane] = ex_warp;
  }
  __syncthreads();

  ex += warp_totals[warp];
  if (gid < n) output[gid] = ex;

  if (threadIdx.x == blockDim.x - 1) {
    block_sums[blockIdx.x] = ex + v;
  }
}

__global__ void add_block_offsets_kernel(int* __restrict__ data,
                                         const int* __restrict__ block_offsets,
                                         int n) {
  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  if (gid < n) data[gid] += block_offsets[blockIdx.x];
}

__global__ void single_block_exclusive_scan_kernel(const int* __restrict__ input,
                                                   int* __restrict__ output,
                                                   int n) {
  __shared__ int warp_totals[WARP_SIZE];

  const int tid = threadIdx.x;
  const int lane = tid & (WARP_SIZE - 1);
  const int warp = tid >> 5;
  const int num_warps = blockDim.x >> 5;

  int v = (tid < n) ? input[tid] : 0;
  int ex = warp_exclusive_scan(v);
  int inclusive = ex + v;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = inclusive;
  __syncthreads();

  if (warp == 0) {
    int t = (lane < num_warps) ? warp_totals[lane] : 0;
    int ex_warp = warp_exclusive_scan(t);
    warp_totals[lane] = ex_warp;
  }
  __syncthreads();

  ex += warp_totals[warp];
  if (tid < n) output[tid] = ex;
}

inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0 || n <= 0) return;

  constexpr int BLOCK = 256;
  const int num_blocks = ceil_div(n, BLOCK);

  int* block_sums = nullptr;
  int* block_offsets = nullptr;

  if (num_blocks > 1) {
    cudaMalloc(&block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
    cudaMalloc(&block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (num_blocks == 1) {
      single_block_exclusive_scan_kernel<<<1, BLOCK>>>(input, output, n);
    } else {
      block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, block_sums, n);
      single_block_exclusive_scan_kernel<<<1, BLOCK>>>(block_sums, block_offsets, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, block_offsets, n);
    }
  }

  if (block_sums) cudaFree(block_sums);
  if (block_offsets) cudaFree(block_offsets);
}
