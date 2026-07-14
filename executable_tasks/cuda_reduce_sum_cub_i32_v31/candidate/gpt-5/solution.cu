#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Warp-level reduction for int using shuffle operations
__device__ __forceinline__ int warp_reduce_sum(int val) {
  unsigned mask = 0xFFFFFFFFu;
  // Reduce within a warp (assumes warpSize == 32)
  val += __shfl_down_sync(mask, val, 16);
  val += __shfl_down_sync(mask, val, 8);
  val += __shfl_down_sync(mask, val, 4);
  val += __shfl_down_sync(mask, val, 2);
  val += __shfl_down_sync(mask, val, 1);
  return val;
}

template <int BLOCK_SIZE>
__global__ __launch_bounds__(BLOCK_SIZE) void block_reduce_kernel(const int* __restrict__ input,
                                                                 int* __restrict__ output,
                                                                 int n) {
  int tid = threadIdx.x;
  int gid = blockIdx.x * BLOCK_SIZE + tid;
  int grid_stride = BLOCK_SIZE * gridDim.x;

  int sum = 0;

  // Grid-stride loop with unrolling by 4 to improve ILP and reduce loop overhead
  int idx = gid;
  int stride = grid_stride;
  // Process in chunks of 4*stride when possible
  int limit = n;
  while (idx < limit) {
    int v0 = input[idx];
    int v1 = 0, v2 = 0, v3 = 0;
    int i1 = idx + stride;
    int i2 = i1 + stride;
    int i3 = i2 + stride;
    if (i1 < limit) v1 = input[i1]; else { sum += v0; break; }
    if (i2 < limit) v2 = input[i2]; else { sum += v0 + v1; break; }
    if (i3 < limit) v3 = input[i3]; else { sum += v0 + v1 + v2; break; }
    sum += v0 + v1 + v2 + v3;
    idx += 4 * stride;
  }

  // Intra-warp reduction
  sum = warp_reduce_sum(sum);

  // Shared memory to hold one partial sum per warp
  __shared__ int warp_sums[32]; // supports up to 1024 threads per block
  int lane = tid & (warpSize - 1);
  int warp_id = tid >> 5; // divide by 32

  if (lane == 0) {
    warp_sums[warp_id] = sum;
  }
  __syncthreads();

  // Final reduction by the first warp
  if (warp_id == 0) {
    int num_warps = (BLOCK_SIZE + warpSize - 1) / warpSize;
    int block_sum = (lane < num_warps) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      atomicAdd(output, block_sum);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Choose launch configuration
  constexpr int kBlock = 256;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;

  // Limit blocks to balance between parallelism and atomic pressure
  int max_blocks = sm_count * 16; // heuristic
  int needed_blocks = (n + kBlock - 1) / kBlock;
  int grid = needed_blocks > 0 ? needed_blocks : 1;
  if (grid > max_blocks) grid = max_blocks;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<kBlock><<<grid, kBlock>>>(input, output, n);
  }
}
