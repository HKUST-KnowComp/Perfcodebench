#include "interface.h"

#include <cuda_runtime.h>

namespace {

#if defined(__CUDA_ARCH__)
__device__ __forceinline__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}
#endif

__global__ void block_reduce_kernel(const int* __restrict__ input,
                                    int* __restrict__ partial,
                                    int n) {
  __shared__ int warp_sums[32];

  int sum = 0;
  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;
  const int stride = blockDim.x * gridDim.x;

  for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += stride) {
    sum += input[idx];
  }

  sum = warp_reduce_sum(sum);

  if (lane == 0) {
    warp_sums[warp] = sum;
  }
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      partial[blockIdx.x] = block_sum;
    }
  }
}

__global__ void final_reduce_kernel(const int* __restrict__ partial,
                                    int* __restrict__ output,
                                    int n) {
  __shared__ int warp_sums[32];

  int sum = 0;
  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;

  for (int idx = tid; idx < n; idx += blockDim.x) {
    sum += partial[idx];
  }

  sum = warp_reduce_sum(sum);

  if (lane == 0) {
    warp_sums[warp] = sum;
  }
  __syncthreads();

  if (warp == 0) {
    int total = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
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

  constexpr int kBlock = 256;
  int grid = (n + kBlock - 1) / kBlock;
  if (grid < 1) grid = 1;
  if (grid > 1024) grid = 1024;

  int* partial = nullptr;
  cudaMalloc(&partial, static_cast<size_t>(grid) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    block_reduce_kernel<<<grid, kBlock>>>(input, partial, n);
    final_reduce_kernel<<<1, kBlock>>>(partial, output, grid);
  }

  cudaFree(partial);
}
