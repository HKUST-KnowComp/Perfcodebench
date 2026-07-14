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

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  int sum = 0;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for (int i = idx; i < n; i += stride) {
    sum += input[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[32];
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;
  int num_warps = blockDim.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < num_warps) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) atomicAdd(output, block_sum);
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  int block = 256;
  int grid = 0;

  cudaDeviceProp prop;
  if (cudaGetDeviceProperties(&prop, 0) == cudaSuccess) {
    int sm = prop.multiProcessorCount;
    grid = sm * 8;
  }
  if (grid <= 0) grid = 256;

  int max_grid = (n + block - 1) / block;
  if (max_grid < 1) max_grid = 1;
  if (grid > max_grid) grid = max_grid;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<<<grid, block>>>(input, output, n);
  }
}
