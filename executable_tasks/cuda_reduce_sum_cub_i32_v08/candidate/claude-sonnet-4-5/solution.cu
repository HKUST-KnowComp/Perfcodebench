#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
  __shared__ int shared[256];
  
  int sum = 0;
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    sum += input[idx];
  }
  
  shared[threadIdx.x] = sum;
  __syncthreads();
  
  // Reduce within block using shared memory
  for (int s = blockDim.x / 2; s > 32; s >>= 1) {
    if (threadIdx.x < s) {
      shared[threadIdx.x] += shared[threadIdx.x + s];
    }
    __syncthreads();
  }
  
  // Final warp reduction
  if (threadIdx.x < 32) {
    int val = shared[threadIdx.x];
    for (int offset = 32; offset > 0; offset >>= 1) {
      val += __shfl_down_sync(0xffffffff, val, offset);
    }
    if (threadIdx.x == 0) {
      atomicAdd(output, val);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
