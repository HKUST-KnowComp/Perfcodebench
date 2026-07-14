#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void block_reduce_kernel(const int* input, int* block_sums, int n) {
  extern __shared__ int shared[];
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int sum = 0;
  while (idx < n) {
    sum += input[idx];
    idx += blockDim.x * gridDim.x;
  }
  shared[threadIdx.x] = sum;
  __syncthreads();

  for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
    if (threadIdx.x < stride) {
      shared[threadIdx.x] += shared[threadIdx.x + stride];
    }
    __syncthreads();
  }

  if (threadIdx.x == 0) {
    block_sums[blockIdx.x] = shared[0];
  }
}

__global__ void reduce_block_sums_kernel(const int* block_sums, int* output) {
  extern __shared__ int shared[];
  int idx = threadIdx.x;
  shared[idx] = block_sums[idx];
  __syncthreads();

  for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
    if (idx < stride) {
      shared[idx] += shared[idx + stride];
    }
    __syncthreads();
  }

  if (threadIdx.x == 0) {
    *output = shared[0];
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  int* block_sums;
  cudaMalloc(&block_sums, grid * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    block_reduce_kernel<<<grid, kBlock, kBlock * sizeof(int)>>>(input, block_sums, n);
    reduce_block_sums_kernel<<<1, 1024, 1024 * sizeof(int)>>>(block_sums, output);
  }

  cudaFree(block_sums);
}