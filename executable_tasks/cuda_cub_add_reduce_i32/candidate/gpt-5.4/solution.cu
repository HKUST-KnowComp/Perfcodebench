#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle instructions.
__device__ __forceinline__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

// Single-kernel reduction: each block accumulates a grid-stride chunk,
// reduces locally, then performs one atomic add per block.
__global__ void block_reduce_kernel(const int* __restrict__ input,
                                    int* __restrict__ output,
                                    int n) {
  __shared__ int warp_sums[8];  // 256 threads / 32 = 8 warps

  int sum = 0;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for (int i = idx; i < n; i += stride) {
    sum += input[i];
  }

  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;

  sum = warp_reduce_sum(sum);
  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) atomicAdd(output, block_sum);
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  constexpr int block_size = 256;
  int grid_size = 0;

  cudaDeviceProp prop;
  int device = 0;
  if (cudaGetDevice(&device) == cudaSuccess &&
      cudaGetDeviceProperties(&prop, device) == cudaSuccess) {
    // Use a moderate multiple of SM count to provide enough work while
    // minimizing atomic contention at the block-output stage.
    grid_size = prop.multiProcessorCount * 4;
  } else {
    grid_size = 256;
  }

  if (n > 0) {
    int max_useful = (n + block_size - 1) / block_size;
    if (max_useful < 1) max_useful = 1;
    if (grid_size > max_useful) grid_size = max_useful;
  } else {
    grid_size = 1;
  }

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<grid_size, block_size>>>(input, output, n);
  }
}
