#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void kernel1(const int* input, int* partial_sums, int n) {
  extern __shared__ int sdata[];
  int tid = threadIdx.x;
  int block_id = blockIdx.x;
  int stride = blockDim.x * gridDim.x;
  int sum = 0;
  for (int idx = block_id * blockDim.x + tid; idx < n; idx += stride) {
    sum += input[idx];
  }
  sdata[tid] = sum;
  __syncthreads();
  for (int s = blockDim.x / 2; s > 0; s >>= 1) {
    if (tid < s) {
      sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
  }
  if (tid == 0) {
    partial_sums[block_id] = sdata[0];
  }
}

__global__ void kernel2(const int* partial_sums, int* output) {
  extern __shared__ int sdata[];
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x + tid;
  sdata[tid] = partial_sums[idx];
  __syncthreads();
  for (int s = blockDim.x / 2; s > 0; s >>= 1) {
    if (tid < s) {
      sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
  }
  if (tid == 0) {
    *output = sdata[0];
  }
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  int* partial_sums;
  cudaMalloc(&partial_sums, 1024 * sizeof(int));
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    kernel1<<<1024, 256, 256 * sizeof(int)>>>(input, partial_sums, n);
    kernel2<<<1, 1024, 1024 * sizeof(int)>>>(partial_sums, output);
  }
  cudaFree(partial_sums);
}