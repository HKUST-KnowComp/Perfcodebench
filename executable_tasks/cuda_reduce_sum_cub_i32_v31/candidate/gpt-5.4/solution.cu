#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kMaxGrid = 1024;

__inline__ __device__ int warp_reduce_sum(int v) {
  for (int offset = 16; offset > 0; offset >>= 1) {
    v += __shfl_down_sync(0xffffffff, v, offset);
  }
  return v;
}

__global__ void reduce_sum_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int n) {
  int sum = 0;

  const int stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  for (; idx < n; idx += stride) {
    sum += input[idx];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[32];
  const int lane = threadIdx.x & 31;
  const int warp = threadIdx.x >> 5;

  if (lane == 0) {
    warp_sums[warp] = sum;
  }
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      atomicAdd(output, block_sum);
    }
  }
}

inline int ceil_div(int a, int b) {
  return (a + b - 1) / b;
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  int grid = ceil_div(n, kBlockSize * 8);
  if (grid < 1) grid = 1;
  if (grid > kMaxGrid) grid = kMaxGrid;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_sum_kernel<<<grid, kBlockSize>>>(input, output, n);
  }
}
