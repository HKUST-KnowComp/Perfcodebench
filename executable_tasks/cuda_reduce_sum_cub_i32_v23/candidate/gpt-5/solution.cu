#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle operations
__inline__ __device__ int warp_reduce_sum(int val) {
  unsigned mask = 0xFFFFFFFFu;
  val += __shfl_down_sync(mask, val, 16);
  val += __shfl_down_sync(mask, val, 8);
  val += __shfl_down_sync(mask, val, 4);
  val += __shfl_down_sync(mask, val, 2);
  val += __shfl_down_sync(mask, val, 1);
  return val;
}

template<int BLOCK_SIZE, int UNROLL>
__launch_bounds__(BLOCK_SIZE, 2)
__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  int tid = threadIdx.x;
  int lane = tid & 31;
  int warpId = tid >> 5;

  int local = 0;

  // Grid-stride loop with unrolling
  int idx = blockIdx.x * BLOCK_SIZE * UNROLL + tid;
  int stride = BLOCK_SIZE * gridDim.x * UNROLL;

  for (int base = idx; base < n; base += stride) {
#pragma unroll
    for (int j = 0; j < UNROLL; ++j) {
      int i = base + j * BLOCK_SIZE;
      if (i < n) {
#if __CUDA_ARCH__ >= 350
        local += __ldg(input + i);
#else
        local += input[i];
#endif
      }
    }
  }

  // Intra-warp reduction
  local = warp_reduce_sum(local);

  // Shared memory for warp results
  __shared__ int warp_sums[BLOCK_SIZE / 32];
  if (lane == 0) {
    warp_sums[warpId] = local;
  }
  __syncthreads();

  // Final reduction by first warp
  if (warpId == 0) {
    int val = 0;
    const int numWarps = BLOCK_SIZE / 32;
    if (tid < numWarps) {
      val = warp_sums[tid];
    }
    val = warp_reduce_sum(val);
    if (tid == 0) {
      // One atomic add per block
      atomicAdd(output, val);
    }
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  constexpr int kUnroll = 8;

  // Choose grid size based on problem size and SM count to reduce atomic pressure
  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;

  // Maximum useful blocks: limit to avoid too many atomics, but keep SMs busy
  int maxBlocks = sm_count * 8; // heuristic: 8 blocks per SM
  long long workPerBlock = static_cast<long long>(kBlock) * kUnroll;
  int grid_by_size = (n > 0) ? static_cast<int>((n + workPerBlock - 1) / workPerBlock) : 1;
  int grid = grid_by_size > maxBlocks ? maxBlocks : (grid_by_size < 1 ? 1 : grid_by_size);

  for (int iter = 0; iter < iters; ++iter) {
    // Zero the output for this iteration
    cudaMemset(output, 0, sizeof(int));

    if (n <= 0) {
      continue; // nothing to sum
    }

    reduce_sum_kernel<kBlock, kUnroll><<<grid, kBlock>>>(input, output, n);
  }
}
