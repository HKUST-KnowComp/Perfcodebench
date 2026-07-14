#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kItemsPerThread = 8;

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
                                    int* __restrict__ output,
                                    int n) {
  __shared__ int warp_sums[kBlockSize / 32];

  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;

  const int block_base = blockIdx.x * (blockDim.x * kItemsPerThread);
  const int thread_base = block_base + tid;
  const int stride = blockDim.x;

  int sum = 0;

#pragma unroll
  for (int i = 0; i < kItemsPerThread; ++i) {
    const int idx = thread_base + i * stride;
    if (idx < n) sum += input[idx];
  }

  sum = warp_reduce_sum(sum);

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (kBlockSize / 32)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) atomicAdd(output, block_sum);
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  const int elems_per_block = kBlockSize * kItemsPerThread;
  int grid = (n + elems_per_block - 1) / elems_per_block;
  if (grid < 1) grid = 1;

  const int max_grid = 4096;
  if (grid > max_grid) grid = max_grid;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<grid, kBlockSize>>>(input, output, n);
  }
}
