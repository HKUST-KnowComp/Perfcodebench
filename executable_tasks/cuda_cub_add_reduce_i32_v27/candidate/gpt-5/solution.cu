#include "interface.h"
#include <cuda_runtime.h>

namespace {

#ifndef BLOCK_DIM
#define BLOCK_DIM 256
#endif
#ifndef UNROLL
#define UNROLL 4
#endif

__forceinline__ __device__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  int tid = threadIdx.x;
  int lane = tid & 31;
  int warp_id = tid >> 5; // warp index within block

  int idx = blockIdx.x * blockDim.x + tid;
  int grid_stride = blockDim.x * gridDim.x;

  int local_sum = 0;

  // Unrolled grid-stride loop
  for (int i = idx; i < n; i += grid_stride * UNROLL) {
    int s = 0;
    int j = i;
    if (j < n) s += __ldg(input + j);
    j += grid_stride;
    if (j < n) s += __ldg(input + j);
    j += grid_stride;
    if (j < n) s += __ldg(input + j);
    j += grid_stride;
    if (j < n) s += __ldg(input + j);
    local_sum += s;
  }

  // In-warp reduction
  local_sum = warp_reduce_sum(local_sum);

  __shared__ int warp_sums[32]; // supports up to 1024 threads per block
  if (lane == 0) warp_sums[warp_id] = local_sum;
  __syncthreads();

  // First warp reduces warp sums
  if (warp_id == 0) {
    int block_sum = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      atomicAdd(output, block_sum);
    }
  }
}

inline int compute_grid_size(int n) {
  if (n <= 0) return 1;
  int dev = 0;
  cudaGetDevice(&dev);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, dev);
  long long blocks = (static_cast<long long>(n) + (BLOCK_DIM * UNROLL - 1)) / (BLOCK_DIM * UNROLL);
  int min_blocks = prop.multiProcessorCount * 8; // keep GPU busy
  if (blocks < min_blocks) blocks = min_blocks;
  if (blocks > 65535) blocks = 65535;
  if (blocks < 1) blocks = 1;
  return static_cast<int>(blocks);
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  const dim3 block(BLOCK_DIM);
  const dim3 grid(compute_grid_size(n));
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemsetAsync(output, 0, sizeof(int));
    reduce_kernel<<<grid, block>>>(input, output, n);
  }
}
