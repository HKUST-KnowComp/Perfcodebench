#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ int warp_reduce_sum(int val) {
  unsigned mask = 0xffffffffu;
#pragma unroll
  for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
    val += __shfl_down_sync(mask, val, offset);
  }
  return val;
}

template<int BLOCK_SIZE>
__launch_bounds__(BLOCK_SIZE, 2)
__global__ void block_reduce_atomic_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  int local = 0;
  int gid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
  int stride = BLOCK_SIZE * gridDim.x;

  // Unroll by 4 for improved memory throughput
  int i = gid;
  int last = n - 4 * stride;
  for (; i <= last; i += 4 * stride) {
    int v0 = input[i];
    int v1 = input[i + stride];
    int v2 = input[i + 2 * stride];
    int v3 = input[i + 3 * stride];
    local += v0 + v1 + v2 + v3;
  }
  for (; i < n; i += stride) {
    local += input[i];
  }

  // In-warp reduction
  local = warp_reduce_sum(local);

  __shared__ int warp_sums[32]; // supports up to 1024 threads per block
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;
  if (lane == 0) {
    warp_sums[warp] = local;
  }
  __syncthreads();

  // Cross-warp reduction by the first warp
  if (warp == 0) {
    int sum = (lane < (BLOCK_SIZE + 31) / 32) ? warp_sums[lane] : 0;
    sum = warp_reduce_sum(sum);
    if (lane == 0) {
      atomicAdd(output, sum);
    }
  }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  const int BLOCK_SIZE = 256;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);
  int sm_count = prop.multiProcessorCount;

  int grid = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
  if (grid < 1) grid = 1;
  int max_blocks = sm_count * 20; // tuning multiplier
  if (grid > max_blocks) grid = max_blocks;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_atomic_kernel<BLOCK_SIZE><<<grid, BLOCK_SIZE>>>(input, output, n);
  }
}
