#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
  __shared__ int sdata[256];
  
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x * 4 + threadIdx.x;
  int gridSize = blockDim.x * gridDim.x * 4;
  
  int sum = 0;
  for (int i = idx; i < n; i += gridSize) {
    sum += input[i];
    if (i + blockDim.x < n) sum += input[i + blockDim.x];
    if (i + blockDim.x * 2 < n) sum += input[i + blockDim.x * 2];
    if (i + blockDim.x * 3 < n) sum += input[i + blockDim.x * 3];
  }
  
  sdata[tid] = sum;
  __syncthreads();
  
  for (int s = blockDim.x / 2; s > 32; s >>= 1) {
    if (tid < s) {
      sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
  }
  
  if (tid < 32) {
    int val = sdata[tid];
    val += __shfl_down_sync(0xffffffff, val, 16);
    val += __shfl_down_sync(0xffffffff, val, 8);
    val += __shfl_down_sync(0xffffffff, val, 4);
    val += __shfl_down_sync(0xffffffff, val, 2);
    val += __shfl_down_sync(0xffffffff, val, 1);
    
    if (tid == 0) {
      atomicAdd(output, val);
    }
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<2048, 256>>>(input, output, n);
  }
}
