#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n, bool init) {
  __shared__ int sdata[256];
  
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int gridSize = blockDim.x * gridDim.x;
  
  // Initialize output once
  if (init && blockIdx.x == 0 && tid == 0) {
    *output = 0;
  }
  if (init) {
    __threadfence();
  }
  
  // Grid-stride loop to accumulate
  int sum = 0;
  for (int i = idx; i < n; i += gridSize) {
    sum += input[i];
  }
  
  sdata[tid] = sum;
  __syncthreads();
  
  // Reduce within block
  for (int s = blockDim.x / 2; s > 32; s >>= 1) {
    if (tid < s) {
      sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
  }
  
  // Warp-level reduction
  if (tid < 32) {
    int val = sdata[tid];
    val += sdata[tid + 32];
    val += sdata[tid + 16];
    val += sdata[tid + 8];
    val += sdata[tid + 4];
    val += sdata[tid + 2];
    val += sdata[tid + 1];
    
    if (tid == 0) {
      atomicAdd(output, val);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = min(1024, (n + kBlock - 1) / kBlock);
  
  for (int iter = 0; iter < iters; ++iter) {
    reduce_kernel<<<grid, kBlock>>>(input, output, n, iter == 0);
  }
}