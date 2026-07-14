#include "interface.h"

#include <cuda_runtime.h>

namespace {

__inline__ __device__ int warp_reduce_sum(int val) {
  #pragma unroll
  for (int offset = 16; offset > 0; offset >>= 1) {
    val += __shfl_down_sync(0xffffffff, val, offset);
  }
  return val;
}

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
  __shared__ int shared[32];
  
  int sum = 0;
  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int wid = tid >> 5;
  
  for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
    sum += input[idx];
  }
  
  sum = warp_reduce_sum(sum);
  
  if (lane == 0) {
    shared[wid] = sum;
  }
  __syncthreads();
  
  if (tid < 32) {
    sum = (tid < (blockDim.x >> 5)) ? shared[tid] : 0;
    sum = warp_reduce_sum(sum);
    
    if (tid == 0) {
      atomicAdd(output, sum);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = min(1024, (n + kBlock - 1) / kBlock);
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
