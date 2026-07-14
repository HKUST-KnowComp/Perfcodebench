#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle operations
__device__ __forceinline__ int warp_reduce_sum(int val) {
  unsigned mask = 0xFFFFFFFF;
  val += __shfl_down_sync(mask, val, 16);
  val += __shfl_down_sync(mask, val, 8);
  val += __shfl_down_sync(mask, val, 4);
  val += __shfl_down_sync(mask, val, 2);
  val += __shfl_down_sync(mask, val, 1);
  return val;
}

// Block-level reduction with one atomicAdd per block
__global__ void block_atomic_reduce_kernel(const int* __restrict__ input,
                                           int* __restrict__ output,
                                           int n) {
  // Grid-stride loop with ILP
  int tid = threadIdx.x;
  int block_threads = blockDim.x;
  int grid_threads = block_threads * gridDim.x;
  int idx = blockIdx.x * block_threads + tid;

  int local_sum = 0;

  // Process 4 grid-stride iterations per loop to increase ILP
  for (int i = idx; i < n; i += grid_threads * 4) {
    int j = i;
    if (j < n) local_sum += __ldg(&input[j]);
    j += grid_threads;
    if (j < n) local_sum += __ldg(&input[j]);
    j += grid_threads;
    if (j < n) local_sum += __ldg(&input[j]);
    j += grid_threads;
    if (j < n) local_sum += __ldg(&input[j]);
  }

  // Intra-warp reduction
  local_sum = warp_reduce_sum(local_sum);

  // Shared memory to hold per-warp sums
  __shared__ int warp_sums[32]; // supports up to 1024 threads (32 warps); we use <= 32 warps
  int lane = tid & 31;          // thread lane within warp
  int warp = tid >> 5;          // warp index within block

  if (lane == 0) {
    warp_sums[warp] = local_sum;
  }
  __syncthreads();

  // Final reduction of warp sums by warp 0
  if (warp == 0) {
    int block_sum = (lane < (block_threads >> 5)) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      atomicAdd(output, block_sum);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;

  // Determine a grid size that saturates the device but avoids excessive atomics
  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);
  int sms = prop.multiProcessorCount;

  // Aim for ~8 blocks per SM, capped by problem size
  int maxBlocks = sms * 8;
  int blocksForN = (n + kBlock - 1) / kBlock;
  int grid = blocksForN > 0 ? blocksForN : 1;
  if (grid > maxBlocks) grid = maxBlocks;
  if (grid < 1) grid = 1;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_atomic_reduce_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
