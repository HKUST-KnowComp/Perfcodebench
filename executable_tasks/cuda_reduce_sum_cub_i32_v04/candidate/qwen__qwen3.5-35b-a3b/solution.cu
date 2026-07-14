#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
  extern __shared__ int sdata[];
  int tid = threadIdx.x;
  int sum = 0;
  
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    sum += input[idx];
  }
  
  sdata[tid] = sum;
  __syncthreads();
  
  for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
    if (tid < offset) {
      sdata[tid] += sdata[tid + offset];
    }
    __syncthreads();
  }
  
  if (tid == 0) {
    atomicAdd(output, sdata[0]);
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = (n + kBlock - 1) / kBlock;
  const int shared_mem = kBlock * sizeof(int);
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_sum_kernel<<<grid, kBlock, shared_mem>>>(input, output, n);
  }
}