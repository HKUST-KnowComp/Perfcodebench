#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ float warp_reduce_sum(float v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

__global__ void reduce_f32_kernel(const float* __restrict__ input,
                                  float* __restrict__ partial,
                                  int n) {
  float sum = 0.0f;

  int tid = threadIdx.x;
  int global_thread = blockIdx.x * blockDim.x + tid;
  int stride = blockDim.x * gridDim.x;

  for (int i = global_thread; i < n; i += stride) {
    sum += input[i];
  }

  sum = warp_reduce_sum(sum);

  __shared__ float warp_sums[32];
  int lane = tid & 31;
  int warp = tid >> 5;
  int warp_count = blockDim.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    sum = (lane < warp_count) ? warp_sums[lane] : 0.0f;
    sum = warp_reduce_sum(sum);
    if (lane == 0) partial[blockIdx.x] = sum;
  }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
  if (n <= 0 || iters <= 0) return 0.0f;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  constexpr int threads = 256;
  int max_blocks = prop.multiProcessorCount * 4;
  int blocks = (n + threads - 1) / threads;
  if (blocks > max_blocks) blocks = max_blocks;
  if (blocks < 1) blocks = 1;

  float* d_partial = nullptr;
  cudaMalloc(&d_partial, static_cast<size_t>(blocks) * sizeof(float));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_f32_kernel<<<blocks, threads>>>(input, d_partial, n);
    int cur_n = blocks;
    while (cur_n > 1) {
      int next_blocks = (cur_n + threads - 1) / threads;
      if (next_blocks > max_blocks) next_blocks = max_blocks;
      if (next_blocks < 1) next_blocks = 1;
      reduce_f32_kernel<<<next_blocks, threads>>>(d_partial, d_partial, cur_n);
      cur_n = next_blocks;
    }
  }

  float actual = 0.0f;
  cudaMemcpy(&actual, d_partial, sizeof(float), cudaMemcpyDeviceToHost);
  cudaFree(d_partial);
  return actual;
}
