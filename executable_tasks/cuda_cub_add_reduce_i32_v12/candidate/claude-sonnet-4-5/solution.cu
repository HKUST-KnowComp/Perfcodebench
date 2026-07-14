#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ int warp_reduce_sum(int val) {
  for (int offset = 16; offset > 0; offset >>= 1)
    val += __shfl_down_sync(0xffffffff, val, offset);
  return val;
}

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
  __shared__ int shared[32];
  
  int sum = 0;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  for (int i = idx; i < n; i += stride) {
    sum += input[i];
  }
  
  int lane = threadIdx.x % 32;
  int wid = threadIdx.x / 32;
  
  sum = warp_reduce_sum(sum);
  
  if (lane == 0) shared[wid] = sum;
  __syncthreads();
  
  if (wid == 0) {
    sum = (threadIdx.x < (blockDim.x / 32)) ? shared[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (threadIdx.x == 0) {
      atomicAdd(output, sum);
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
