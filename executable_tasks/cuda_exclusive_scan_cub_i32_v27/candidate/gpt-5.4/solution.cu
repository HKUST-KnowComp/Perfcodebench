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
  __shared__ int warp_totals[32];

  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  const int lane = threadIdx.x & (WARP_SIZE - 1);
  const int warp = threadIdx.x >> 5;
  const int warps_per_block = blockDim.x >> 5;

  int v = (gid < n) ? input[gid] : 0;
  int excl = warp_exclusive_scan(v);
  int incl = excl + v;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = incl;
  __syncthreads();

  int warp_prefix = 0;
  if (warp == 0) {
    int t = (lane < warps_per_block) ? warp_totals[lane] : 0;
    int e = warp_exclusive_scan(t);
    if (lane < warps_per_block) warp_totals[lane] = e;
  }
  __syncthreads();

  warp_prefix = warp_totals[warp];
  if (gid < n) output[gid] = excl + warp_prefix;

  if (threadIdx.x == blockDim.x - 1) {
    block_sums[blockIdx.x] = warp_prefix + incl;
  }
}

__global__ void add_offsets_kernel(int* __restrict__ output,
                                   const int* __restrict__ block_offsets,
                                   int n) {
  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  if (gid < n) {
    output[gid] += block_offsets[blockIdx.x];
  }
}

__global__ void single_block_exclusive_scan_kernel(const int* __restrict__ input,
                                                   int* __restrict__ output,
                                                   int n) {
  __shared__ int warp_totals[32];
  const int tid = threadIdx.x;
  const int lane = tid & (WARP_SIZE - 1);
  const int warp = tid >> 5;
  const int warps_per_block = blockDim.x >> 5;

  int v = (tid < n) ? input[tid] : 0;
  int excl = warp_exclusive_scan(v);
  int incl = excl + v;

  if (lane == WARP_SIZE - 1) warp_totals[warp] = incl;
  __syncthreads();

  if (warp == 0) {
    int t = (lane < warps_per_block) ? warp_totals[lane] : 0;
    int e = warp_exclusive_scan(t);
    if (lane < warps_per_block) warp_totals[lane] = e;
  }
  __syncthreads();

  if (tid < n) output[tid] = excl + warp_totals[warp];
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

  const int num_blocks = (n + BLOCK - 1) / BLOCK;

  int* d_block_sums = nullptr;
  int* d_block_offsets = nullptr;
  cudaMalloc(&d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  cudaMalloc(&d_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));

  if (num_blocks <= BLOCK) {
    for (int iter = 0; iter < iters; ++iter) {
      block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, d_block_sums, n);
      single_block_exclusive_scan_kernel<<<1, BLOCK>>>(d_block_sums, d_block_offsets, num_blocks);
      add_offsets_kernel<<<num_blocks, BLOCK>>>(output, d_block_offsets, n);
    }
  } else {
    int* h_block_sums = new int[num_blocks];
    int* h_block_offsets = new int[num_blocks];

    for (int iter = 0; iter < iters; ++iter) {
      block_scan_kernel<<<num_blocks, BLOCK>>>(input, output, d_block_sums, n);
      cudaMemcpy(h_block_sums, d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int), cudaMemcpyDeviceToHost);
      int running = 0;
      for (int i = 0; i < num_blocks; ++i) {
        h_block_offsets[i] = running;
        running += h_block_sums[i];
      }
      cudaMemcpy(d_block_offsets, h_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int), cudaMemcpyHostToDevice);
      add_offsets_kernel<<<num_blocks, BLOCK>>>(output, d_block_offsets, n);
    }

    delete[] h_block_sums;
    delete[] h_block_offsets;
  }

  cudaFree(d_block_sums);
  cudaFree(d_block_offsets);
}
