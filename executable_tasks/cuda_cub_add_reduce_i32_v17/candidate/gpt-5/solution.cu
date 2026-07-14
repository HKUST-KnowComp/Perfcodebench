#include "interface.h"
#include <cuda_runtime.h>

namespace {

#ifndef WARP_SIZE
#define WARP_SIZE 32
#endif

static constexpr int BLOCK_SIZE = 256;

__forceinline__ __device__ unsigned int warp_reduce_sum(unsigned int val) {
  unsigned mask = 0xFFFFFFFFu;
  for (int offset = WARP_SIZE / 2; offset > 0; offset >>= 1) {
    val += __shfl_down_sync(mask, val, offset);
  }
  return val;
}

__launch_bounds__(BLOCK_SIZE, 2)
__global__ void reduce_add_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  // Accumulate per-thread partial sum in unsigned to guarantee well-defined wrap-around
  unsigned int local = 0u;

  // Use 64-bit indexing to safely handle large n
  long long tid = threadIdx.x;
  long long bid = blockIdx.x;
  long long bdim = blockDim.x;
  long long gdim = gridDim.x;

  long long idx = bid * (bdim * 2LL) + tid;
  long long stride = gdim * (bdim * 2LL);

  while (idx < n) {
    unsigned int a = static_cast<unsigned int>(__ldg(input + idx));
    unsigned int b = 0u;
    long long j = idx + bdim;
    if (j < n) b = static_cast<unsigned int>(__ldg(input + j));
    local += a + b;
    idx += stride;
  }

  // Intra-warp reduction
  local = warp_reduce_sum(local);

  // Shared memory to hold warp-level sums
  __shared__ unsigned int warp_sums[BLOCK_SIZE / WARP_SIZE];
  int lane = threadIdx.x & (WARP_SIZE - 1);
  int warp_id = threadIdx.x >> 5; // / WARP_SIZE

  if (lane == 0) warp_sums[warp_id] = local;
  __syncthreads();

  // Final reduction by first warp
  if (warp_id == 0) {
    unsigned int block_sum = (lane < (BLOCK_SIZE / WARP_SIZE)) ? warp_sums[lane] : 0u;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      // Atomic add modulo 2^32 via unsigned int pointer
      atomicAdd(reinterpret_cast<unsigned int*>(output), block_sum);
    }
  }
}

inline int compute_grid_size(int n, int block_size) {
  if (n <= 0) return 1;
  int elems_per_block = block_size * 2; // two elements per thread per iteration
  int max_blocks_from_n = (n + elems_per_block - 1) / elems_per_block;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop{};
  cudaGetDeviceProperties(&prop, device);
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;

  // Oversubscribe SMs to hide latency but cap grid size to avoid excessive atomics
  int blocks_by_sm = sm_count * 32; // tune: 32 blocks per SM

  int grid = max_blocks_from_n;
  if (grid > blocks_by_sm) grid = blocks_by_sm;

  // Cap to legacy 1D grid limit for wide compatibility
  const int grid_cap = 65535;
  if (grid > grid_cap) grid = grid_cap;
  if (grid < 1) grid = 1;
  return grid;
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  int grid = compute_grid_size(n, BLOCK_SIZE);
  dim3 block(BLOCK_SIZE);
  dim3 gridDim(grid);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_add_kernel<<<gridDim, block>>>(input, output, n);
  }
}
