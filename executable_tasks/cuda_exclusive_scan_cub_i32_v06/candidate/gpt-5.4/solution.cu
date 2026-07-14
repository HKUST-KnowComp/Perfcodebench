#include "interface.h"

#include <cuda_runtime.h>

namespace {

inline int div_up(int a, int b) { return (a + b - 1) / b; }

__global__ void block_exclusive_scan_kernel(const int* __restrict__ input,
                                            int* __restrict__ output,
                                            int* __restrict__ block_sums,
                                            int n) {
  extern __shared__ int s[];
  const int tid = threadIdx.x;
  const int gid = blockIdx.x * blockDim.x + tid;

  int x = 0;
  if (gid < n) x = input[gid];
  s[tid] = x;
  __syncthreads();

  for (int offset = 1; offset < blockDim.x; offset <<= 1) {
    int v = 0;
    if (tid >= offset) v = s[tid - offset];
    __syncthreads();
    if (tid >= offset) s[tid] += v;
    __syncthreads();
  }

  if (gid < n) output[gid] = s[tid] - x;

  if (tid == blockDim.x - 1) {
    block_sums[blockIdx.x] = s[tid];
  }
}

__global__ void add_block_offsets_kernel(int* __restrict__ output,
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
  extern __shared__ int s[];
  const int tid = threadIdx.x;
  int x = 0;
  if (tid < n) x = input[tid];
  s[tid] = x;
  __syncthreads();

  for (int offset = 1; offset < blockDim.x; offset <<= 1) {
    int v = 0;
    if (tid >= offset) v = s[tid - offset];
    __syncthreads();
    if (tid >= offset) s[tid] += v;
    __syncthreads();
  }

  if (tid < n) output[tid] = s[tid] - x;
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  constexpr int BLOCK = 256;
  const int num_blocks = div_up(n, BLOCK);

  int* block_sums = nullptr;
  int* block_offsets = nullptr;

  if (num_blocks > 1) {
    cudaMalloc(&block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
    cudaMalloc(&block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (num_blocks == 1) {
      single_block_exclusive_scan_kernel<<<1, BLOCK, BLOCK * sizeof(int)>>>(input, output, n);
    } else {
      block_exclusive_scan_kernel<<<num_blocks, BLOCK, BLOCK * sizeof(int)>>>(input, output, block_sums, n);
      single_block_exclusive_scan_kernel<<<1, BLOCK, BLOCK * sizeof(int)>>>(block_sums, block_offsets, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK>>>(output, block_offsets, n);
    }
  }

  if (block_sums) cudaFree(block_sums);
  if (block_offsets) cudaFree(block_offsets);
}
