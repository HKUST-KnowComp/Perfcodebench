#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kMaxBlocks = 1024;

__inline__ __device__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

__global__ void block_reduce_kernel(const int* __restrict__ input,
                                    int* __restrict__ partial,
                                    int n) {
  int sum = 0;
  const int stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  for (; idx < n; idx += stride) {
    sum += input[idx];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[kBlockSize / 32];
  const int lane = threadIdx.x & 31;
  const int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) partial[blockIdx.x] = block_sum;
  }
}

__global__ void final_reduce_kernel(const int* __restrict__ partial,
                                    int* __restrict__ output,
                                    int n) {
  int sum = 0;
  for (int idx = threadIdx.x; idx < n; idx += blockDim.x) {
    sum += partial[idx];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[kBlockSize / 32];
  const int lane = threadIdx.x & 31;
  const int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int total = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    total = warp_reduce_sum(total);
    if (lane == 0) *output = total;
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  int grid = (n + kBlockSize - 1) / kBlockSize;
  if (grid < 1) grid = 1;
  if (grid > kMaxBlocks) grid = kMaxBlocks;

  int* partial = nullptr;
  cudaMalloc(&partial, static_cast<size_t>(grid) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    block_reduce_kernel<<<grid, kBlockSize>>>(input, partial, n);
    final_reduce_kernel<<<1, kBlockSize>>>(partial, output, grid);
  }

  cudaFree(partial);
}
