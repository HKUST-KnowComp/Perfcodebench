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

__global__ void reduce_kernel(const float* __restrict__ input,
                              float* __restrict__ partial,
                              int n) {
  float sum = 0.0f;

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  int idx = blockIdx.x * (block_threads * 2) + tid;
  const int stride = gridDim.x * (block_threads * 2);

  while (idx < n) {
    sum += input[idx];
    const int idx2 = idx + block_threads;
    if (idx2 < n) sum += input[idx2];
    idx += stride;
  }

  sum = warp_reduce_sum(sum);

  __shared__ float warp_sums[32];
  const int lane = tid & (WARP_SIZE - 1);
  const int warp_id = tid >> 5;
  const int warp_count = (block_threads + WARP_SIZE - 1) / WARP_SIZE;

  if (lane == 0) warp_sums[warp_id] = sum;
  __syncthreads();

  if (warp_id == 0) {
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

  const int threads = 256;
  int blocks = (n + threads * 2 - 1) / (threads * 2);
  if (blocks < 1) blocks = 1;

  const int max_blocks = prop.multiProcessorCount * 8;
  if (blocks > max_blocks) blocks = max_blocks;

  float* d_partial = nullptr;
  cudaMalloc(&d_partial, static_cast<size_t>(blocks) * sizeof(float));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_kernel<<<blocks, threads>>>(input, d_partial, n);

    int cur_n = blocks;
    while (cur_n > 1) {
      int next_blocks = (cur_n + threads * 2 - 1) / (threads * 2);
      if (next_blocks < 1) next_blocks = 1;
      reduce_kernel<<<next_blocks, threads>>>(d_partial, d_partial, cur_n);
      cur_n = next_blocks;
    }
  }

  float actual = 0.0f;
  cudaMemcpy(&actual, d_partial, sizeof(float), cudaMemcpyDeviceToHost);
  cudaFree(d_partial);
  return actual;
}
