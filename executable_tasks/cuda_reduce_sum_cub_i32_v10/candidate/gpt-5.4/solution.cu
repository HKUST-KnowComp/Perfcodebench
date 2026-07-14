#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kWarpSize = 32;

__device__ __forceinline__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

__global__ void reduce_stage1_kernel(const int* __restrict__ input,
                                     int* __restrict__ partial,
                                     int n) {
  int sum = 0;
  const int stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  for (; idx < n; idx += stride) {
    sum += input[idx];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[kBlockSize / kWarpSize];
  const int lane = threadIdx.x & (kWarpSize - 1);
  const int warp = threadIdx.x >> 5;

  if (lane == 0) {
    warp_sums[warp] = sum;
  }
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x / kWarpSize)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      partial[blockIdx.x] = block_sum;
    }
  }
}

__global__ void reduce_stage2_kernel(const int* __restrict__ partial,
                                     int* __restrict__ output,
                                     int n) {
  int sum = 0;
  for (int idx = threadIdx.x; idx < n; idx += blockDim.x) {
    sum += partial[idx];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[kBlockSize / kWarpSize];
  const int lane = threadIdx.x & (kWarpSize - 1);
  const int warp = threadIdx.x >> 5;

  if (lane == 0) {
    warp_sums[warp] = sum;
  }
  __syncthreads();

  if (warp == 0) {
    int total = (lane < (blockDim.x / kWarpSize)) ? warp_sums[lane] : 0;
    total = warp_reduce_sum(total);
    if (lane == 0) {
      *output = total;
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  int grid = prop.multiProcessorCount * 4;
  if (grid < 1) grid = 1;

  const int max_blocks_for_n = (n + kBlockSize - 1) / kBlockSize;
  if (grid > max_blocks_for_n) grid = max_blocks_for_n;
  if (grid < 1) grid = 1;

  int* partial = nullptr;
  cudaMalloc(&partial, static_cast<size_t>(grid) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_stage1_kernel<<<grid, kBlockSize>>>(input, partial, n);
    reduce_stage2_kernel<<<1, kBlockSize>>>(partial, output, grid);
  }

  cudaFree(partial);
}
