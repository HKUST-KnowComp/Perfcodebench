#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  int sum = 0;
  for (int i = idx; i < n; i += stride) {
    sum += input[i];
  }

  for (int offset = warpSize / 2; offset > 0; offset /= 2) {
    sum += __shfl_down_sync(0xffffffff, sum, offset);
  }

  __shared__ int warp_sums[32];
  int lane = tid % warpSize;
  int warp_id = tid / warpSize;

  if (lane == 0) {
    warp_sums[warp_id] = sum;
  }
  __syncthreads();

  if (warp_id == 0) {
    int num_warps = (blockDim.x + warpSize - 1) / warpSize;
    int block_sum = (tid < num_warps) ? warp_sums[tid] : 0;
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
      block_sum += __shfl_down_sync(0xffffffff, block_sum, offset);
    }
    if (tid == 0) {
      atomicAdd(output, block_sum);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0) return;
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
