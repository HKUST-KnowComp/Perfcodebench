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

  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;
  const int warps_per_block = blockDim.x >> 5;

  int idx = blockIdx.x * blockDim.x * 4 + tid;
  const int stride = gridDim.x * blockDim.x * 4;

  for (; idx < n; idx += stride) {
    if (idx < n) sum += input[idx];
    if (idx + blockDim.x < n) sum += input[idx + blockDim.x];
    if (idx + 2 * blockDim.x < n) sum += input[idx + 2 * blockDim.x];
    if (idx + 3 * blockDim.x < n) sum += input[idx + 3 * blockDim.x];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[8];
  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    sum = (lane < warps_per_block) ? warp_sums[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (lane == 0) atomicAdd(output, sum);
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  constexpr int block_size = 256;
  int grid = (n + block_size * 4 - 1) / (block_size * 4);
  if (grid < 1) grid = 1;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);
  int max_blocks = prop.multiProcessorCount * 8;
  if (grid > max_blocks) grid = max_blocks;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_i32_kernel<<<grid, block_size>>>(input, output, n);
  }
}
