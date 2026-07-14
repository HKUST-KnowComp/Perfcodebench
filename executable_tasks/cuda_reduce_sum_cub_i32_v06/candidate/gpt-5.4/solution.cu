#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kItemsPerThread = 8;

__inline__ __device__ int warp_reduce_sum(int v) {
  for (int offset = 16; offset > 0; offset >>= 1) {
    v += __shfl_down_sync(0xffffffff, v, offset);
  }
  return v;
}

__global__ void reduce_sum_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int n) {
  int local = 0;

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  const int global_thread = blockIdx.x * block_threads + tid;
  const int stride = gridDim.x * block_threads * kItemsPerThread;
  int idx = global_thread;

  while (idx < n) {
#pragma unroll
    for (int i = 0; i < kItemsPerThread; ++i) {
      int j = idx + i * gridDim.x * block_threads;
      if (j < n) local += input[j];
    }
    idx += stride;
  }

  local = warp_reduce_sum(local);

  __shared__ int warp_sums[32];
  const int lane = tid & 31;
  const int warp = tid >> 5;
  const int warp_count = (block_threads + 31) >> 5;

  if (lane == 0) warp_sums[warp] = local;
  __syncthreads();

  int block_sum = 0;
  if (warp == 0) {
    block_sum = (lane < warp_count) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) atomicAdd(output, block_sum);
  }
}

inline int ceil_div(int a, int b) {
  return (a + b - 1) / b;
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

  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int grid = ceil_div(n, kBlockSize * kItemsPerThread);
  int max_grid = sm_count * 8;
  if (grid < 1) grid = 1;
  if (grid > max_grid) grid = max_grid;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_sum_kernel<<<grid, kBlockSize>>>(input, output, n);
  }
}
