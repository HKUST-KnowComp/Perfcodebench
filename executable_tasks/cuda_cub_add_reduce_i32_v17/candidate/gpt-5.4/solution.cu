#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

__global__ void reduce_i32_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int n) {
  int sum = 0;

  const int stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  for (; idx < n; idx += stride) {
    sum += input[idx];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[8];
  const int lane = threadIdx.x & 31;
  const int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) atomicAdd(output, block_sum);
  }
}

inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  constexpr int threads = 256;

  int blocks = ceil_div(n, threads * 8);
  if (blocks < 1) blocks = 1;
  if (blocks > 1024) blocks = 1024;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_i32_kernel<<<blocks, threads>>>(input, output, n);
  }
}
