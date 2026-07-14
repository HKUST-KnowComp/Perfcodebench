#include "interface.h"

#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warp_reduce_sum(int val) {
  #pragma unroll
  for (int offset = 16; offset > 0; offset >>= 1) {
    val += __shfl_down_sync(0xffffffff, val, offset);
  }
  return val;
}

__global__ void block_reduce_kernel(const int* input, int* partial_sums, int n) {
  __shared__ int shared[32];
  
  int sum = 0;
  const int lane = threadIdx.x & 31;
  const int warp_id = threadIdx.x >> 5;
  
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    sum += input[idx];
  }
  
  sum = warp_reduce_sum(sum);
  
  if (lane == 0) {
    shared[warp_id] = sum;
  }
  __syncthreads();
  
  if (warp_id == 0) {
    sum = (threadIdx.x < (blockDim.x >> 5)) ? shared[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (threadIdx.x == 0) {
      partial_sums[blockIdx.x] = sum;
    }
  }
}

__global__ void final_reduce_kernel(const int* partial_sums, int* output, int num_blocks) {
  __shared__ int shared[32];
  
  int sum = 0;
  const int lane = threadIdx.x & 31;
  const int warp_id = threadIdx.x >> 5;
  
  for (int idx = threadIdx.x; idx < num_blocks; idx += blockDim.x) {
    sum += partial_sums[idx];
  }
  
  sum = warp_reduce_sum(sum);
  
  if (lane == 0) {
    shared[warp_id] = sum;
  }
  __syncthreads();
  
  if (warp_id == 0) {
    sum = (threadIdx.x < (blockDim.x >> 5)) ? shared[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (threadIdx.x == 0) {
      *output = sum;
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 512;
  
  int* partial_sums;
  cudaMalloc(&partial_sums, grid * sizeof(int));
  
  for (int iter = 0; iter < iters; ++iter) {
    block_reduce_kernel<<<grid, kBlock>>>(input, partial_sums, n);
    final_reduce_kernel<<<1, kBlock>>>(partial_sums, output, grid);
  }
  
  cudaFree(partial_sums);
}
