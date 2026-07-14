#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle intrinsics
__forceinline__ __device__ int warp_reduce_sum(int val) {
  unsigned mask = 0xffffffffu;
  val += __shfl_down_sync(mask, val, 16);
  val += __shfl_down_sync(mask, val, 8);
  val += __shfl_down_sync(mask, val, 4);
  val += __shfl_down_sync(mask, val, 2);
  val += __shfl_down_sync(mask, val, 1);
  return val;
}

template<int BLOCK_SIZE>
__launch_bounds__(BLOCK_SIZE, 2)
__global__ void reduce_add_kernel(const int* __restrict__ input, int n, int* __restrict__ output) {
  // Per-thread accumulation in 32-bit preserves modulo 2^32 semantics.
  int local = 0;

  int tid = threadIdx.x;
  int block_threads = BLOCK_SIZE;
  int gid = blockIdx.x * block_threads + tid;
  int stride = gridDim.x * block_threads;

  // Unroll by 4 for better ILP and fewer loop overheads
  int i = gid;
  int limit = n - 4 * stride;
  for (; i <= limit; i += 4 * stride) {
    int a = input[i];
    int b = input[i + stride];
    int c = input[i + 2 * stride];
    int d = input[i + 3 * stride];
    local += a + b + c + d;
  }
  for (; i < n; i += stride) {
    local += input[i];
  }

  // Block-level reduction via warp shuffles
  local = warp_reduce_sum(local);

  __shared__ int warp_sums[32]; // supports up to 1024 threads
  int lane = tid & 31;
  int wid = tid >> 5;

  if (lane == 0) {
    warp_sums[wid] = local;
  }
  __syncthreads();

  int block_sum = 0;
  int num_warps = (BLOCK_SIZE + 31) >> 5;
  if (wid == 0) {
    block_sum = (lane < num_warps) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
  }

  if (tid == 0) {
    atomicAdd(output, block_sum);
  }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  const int BLOCK_SIZE = 256;
  // Choose grid size: cap to avoid too many blocks; ensure at least 1.
  int blocks = (n + (BLOCK_SIZE * 4 - 1)) / (BLOCK_SIZE * 4);
  if (blocks <= 0) blocks = 1;
  if (blocks > 1024) blocks = 1024;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_add_kernel<BLOCK_SIZE><<<blocks, BLOCK_SIZE>>>(input, n, output);
  }
}
