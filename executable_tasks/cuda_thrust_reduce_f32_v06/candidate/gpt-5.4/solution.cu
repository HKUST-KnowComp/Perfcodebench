#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ float warp_reduce_sum(float v) {
  for (int offset = 16; offset > 0; offset >>= 1) {
    v += __shfl_down_sync(0xffffffff, v, offset);
  }
  return v;
}

__global__ void reduce_kernel(const float* __restrict__ input,
                              float* __restrict__ partial,
                              int n) {
  float sum = 0.0f;

  const int global_thread = blockIdx.x * blockDim.x + threadIdx.x;
  const int stride = blockDim.x * gridDim.x;

  for (int i = global_thread; i < n; i += stride) {
    sum += input[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ float warp_sums[32];
  const int lane = threadIdx.x & 31;
  const int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    const int warp_count = blockDim.x >> 5;
    sum = (lane < warp_count) ? warp_sums[lane] : 0.0f;
    sum = warp_reduce_sum(sum);
    if (lane == 0) partial[blockIdx.x] = sum;
  }
}

__global__ void final_reduce_kernel(const float* __restrict__ partial,
                                    float* __restrict__ output,
                                    int n) {
  float sum = 0.0f;

  for (int i = threadIdx.x; i < n; i += blockDim.x) {
    sum += partial[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ float warp_sums[32];
  const int lane = threadIdx.x & 31;
  const int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    const int warp_count = blockDim.x >> 5;
    sum = (lane < warp_count) ? warp_sums[lane] : 0.0f;
    sum = warp_reduce_sum(sum);
    if (lane == 0) *output = sum;
  }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
  if (n <= 0 || iters <= 0) return 0.0f;

  constexpr int threads = 256;
  int blocks = (n + threads - 1) / threads;
  if (blocks > 1024) blocks = 1024;
  if (blocks < 1) blocks = 1;

  float* d_out = nullptr;
  float* d_partial = nullptr;
  cudaMalloc(&d_out, sizeof(float));
  cudaMalloc(&d_partial, static_cast<size_t>(blocks) * sizeof(float));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_kernel<<<blocks, threads>>>(input, d_partial, n);
    final_reduce_kernel<<<1, threads>>>(d_partial, d_out, blocks);
  }

  float actual = 0.0f;
  cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

  cudaFree(d_partial);
  cudaFree(d_out);
  return actual;
}
