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
  
  for (int s = blockDim.x / 2; s > 32; s >>= 1) {
    if (threadIdx.x < s) {
      shared[threadIdx.x] += shared[threadIdx.x + s];
    }
    __syncthreads();
  }
  
  if (threadIdx.x < 32) {
    int warp_sum = shared[threadIdx.x];
    for (int offset = 16; offset > 0; offset >>= 1) {
      warp_sum += __shfl_down_sync(0xffffffff, warp_sum, offset);
    }
    if (threadIdx.x == 0) {
      atomicAdd(output, warp_sum);
    }
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  cudaMemset(output, 0, sizeof(int));
  for (int iter = 0; iter < iters; ++iter) {
    if (iter > 0) cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<2048, 256>>>(input, output, n);
  }
}
