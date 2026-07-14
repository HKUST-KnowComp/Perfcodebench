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

__global__ void reduce_blocks_kernel(const int* __restrict__ input,
                                     int* __restrict__ partials,
                                     int n) {
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
    sum = (lane < num_warps) ? warp_sums[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (lane == 0) partials[blockIdx.x] = sum;
  }
}

__global__ void finalize_kernel(const int* __restrict__ partials,
                                int* __restrict__ output,
                                int n) {
  int sum = 0;
  for (int i = threadIdx.x; i < n; i += blockDim.x) {
    sum += partials[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[32];
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;
  int num_warps = blockDim.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    sum = (lane < num_warps) ? warp_sums[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (lane == 0) *output = sum;
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
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

  const int block_size = 256;
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int grid_size = sm_count * 8;
  int max_blocks = (n + block_size - 1) / block_size;
  if (grid_size > max_blocks) grid_size = max_blocks;
  if (grid_size < 1) grid_size = 1;

  int* partials = nullptr;
  cudaMalloc(&partials, static_cast<size_t>(grid_size) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_blocks_kernel<<<grid_size, block_size>>>(input, partials, n);
    finalize_kernel<<<1, 256>>>(partials, output, grid_size);
  }

  cudaFree(partials);
}
