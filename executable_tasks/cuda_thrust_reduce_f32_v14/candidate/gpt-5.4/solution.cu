#include "interface.h"
#include <cuda_runtime.h>

namespace {

#ifndef WARP_SIZE
#define WARP_SIZE 32
#endif

__inline__ __device__ float warp_reduce_sum(float v) {
  for (int offset = WARP_SIZE / 2; offset > 0; offset >>= 1) {
    v += __shfl_down_sync(0xffffffff, v, offset);
  }
  return v;
}

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
  float sum = 0.0f;
  const int tid = threadIdx.x;
  const int global_thread = blockIdx.x * blockDim.x + tid;
  const int stride = blockDim.x * gridDim.x;

  for (int i = global_thread; i < n; i += stride) {
    sum += input[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ float warp_sums[32];
  const int lane = tid & (WARP_SIZE - 1);
  const int warp = tid >> 5;
  const int num_warps = blockDim.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    sum = (lane < num_warps) ? warp_sums[lane] : 0.0f;
    sum = warp_reduce_sum(sum);
    if (lane == 0) partial[blockIdx.x] = sum;
  }
}

__global__ void reduce_stage2_kernel(const float* __restrict__ partial,
                                     float* __restrict__ output,
                                     int n) {
  float sum = 0.0f;
  const int tid = threadIdx.x;

  for (int i = tid; i < n; i += blockDim.x) {
    sum += partial[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ float warp_sums[32];
  const int lane = tid & (WARP_SIZE - 1);
  const int warp = tid >> 5;
  const int num_warps = blockDim.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    sum = (lane < num_warps) ? warp_sums[lane] : 0.0f;
    sum = warp_reduce_sum(sum);
    if (lane == 0) *output = sum;
  }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
  if (n <= 0 || iters <= 0) return 0.0f;

  cudaDeviceProp prop;
  int device = 0;
  cudaGetDevice(&device);
  cudaGetDeviceProperties(&prop, device);

  constexpr int block_size = 256;
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int grid_size = (n + block_size - 1) / block_size;
  int max_blocks = sm_count * 8;
  if (grid_size > max_blocks) grid_size = max_blocks;
  if (grid_size < 1) grid_size = 1;

  float* d_partial = nullptr;
  float* d_out = nullptr;
  cudaMalloc(&d_partial, static_cast<size_t>(grid_size) * sizeof(float));
  cudaMalloc(&d_out, sizeof(float));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_stage1_kernel<<<grid_size, block_size>>>(input, d_partial, n);
    reduce_stage2_kernel<<<1, block_size>>>(d_partial, d_out, grid_size);
  }

  float actual = 0.0f;
  cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

  cudaFree(d_partial);
  cudaFree(d_out);
  return actual;
}
