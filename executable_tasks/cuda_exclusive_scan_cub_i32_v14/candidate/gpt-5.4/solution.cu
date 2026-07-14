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
                                  int* __restrict__ block_sums,
                                  int n) {
  __shared__ int warp_totals[WARP_SIZE];

  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  const int lane = threadIdx.x & (WARP_SIZE - 1);
  const int warp = threadIdx.x >> 5;
  const int num_warps = blockDim.x >> 5;

  int x = (gid < n) ? input[gid] : 0;
  int incl = warp_inclusive_scan(x);

  if (lane == WARP_SIZE - 1) warp_totals[warp] = incl;
  __syncthreads();

  if (warp == 0) {
    int wv = (lane < num_warps) ? warp_totals[lane] : 0;
    int wincl = warp_inclusive_scan(wv);
    warp_totals[lane] = wincl - wv;
  }
  __syncthreads();

  int block_prefix = warp_totals[warp];
  int excl = block_prefix + incl - x;

  if (gid < n) output[gid] = excl;

  if (threadIdx.x == blockDim.x - 1) {
    block_sums[blockIdx.x] = block_prefix + incl;
  }
}

__global__ void add_offsets_kernel(int* __restrict__ data,
                                   const int* __restrict__ block_offsets,
                                   int n) {
  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  if (gid < n) {
    data[gid] += block_offsets[blockIdx.x];
  }
}

__global__ void scan_block_sums_single_kernel(const int* __restrict__ input,
                                              int* __restrict__ output,
                                              int count) {
  if (blockIdx.x != 0 || threadIdx.x != 0) return;
  int running = 0;
  for (int i = 0; i < count; ++i) {
    output[i] = running;
    running += input[i];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  constexpr int BLOCK = 256;
  const int num_blocks = (n + BLOCK - 1) / BLOCK;

  int* d_block_sums = nullptr;
  int* d_block_offsets = nullptr;

  if (num_blocks > 1) {
    cudaMalloc(&d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
    cudaMalloc(&d_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (num_blocks == 1) {
      block_scan_kernel<<<1, BLOCK>>>(input, output, output, n);
    } else {
      block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, d_block_sums, n);
      scan_block_sums_single_kernel<<<1, 1>>>(d_block_sums, d_block_offsets, num_blocks);
      add_offsets_kernel<<<num_blocks, BLOCK>>>(output, d_block_offsets, n);
    }
  }

  if (d_block_sums) cudaFree(d_block_sums);
  if (d_block_offsets) cudaFree(d_block_offsets);
}
