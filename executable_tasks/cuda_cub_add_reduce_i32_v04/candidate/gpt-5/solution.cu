#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle operations
__inline__ __device__ int warp_reduce_sum(int val) {
  unsigned mask = 0xffffffffu;
  val += __shfl_down_sync(mask, val, 16);
  val += __shfl_down_sync(mask, val, 8);
  val += __shfl_down_sync(mask, val, 4);
  val += __shfl_down_sync(mask, val, 2);
  val += __shfl_down_sync(mask, val, 1);
  return val;
}

template<int UNROLL>
__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
  int tid = threadIdx.x;
  int lane = tid & 31;
  int warp_id = tid >> 5;
  const int total_threads = blockDim.x * gridDim.x;

  int local = 0;
  // Grid-stride loop with unrolling
  for (int base = blockIdx.x * blockDim.x + tid; base < n; base += total_threads * UNROLL) {
    int idx = base;
    #pragma unroll
    for (int k = 0; k < UNROLL; ++k) {
      if (idx < n) {
        local += input[idx];
      }
      idx += total_threads;
    }
  }

  // Intra-warp reduction
  local = warp_reduce_sum(local);

  __shared__ int warp_sums[32]; // supports up to 1024 threads per block
  if (lane == 0) {
    warp_sums[warp_id] = local;
  }
  __syncthreads();

  // First warp reduces the per-warp sums
  int block_sum = 0;
  if (warp_id == 0) {
    int num_warps = (blockDim.x + 31) >> 5;
    block_sum = (lane < num_warps) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) {
      atomicAdd(output, block_sum);
    }
  }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  // Choose launch parameters
  const int block_dim = 256;
  int device = 0;
  cudaDeviceProp prop;
  if (cudaGetDevice(&device) == cudaSuccess) {
    cudaGetDeviceProperties(&prop, device);
  } else {
    // Fallback in unlikely case
    prop.multiProcessorCount = 80;
  }

  // Each thread processes UNROLL elements per outer stride
  constexpr int UNROLL = 8;

  // Estimate grid size: bound by SM count to limit atomics and maximize residency
  int max_blocks = prop.multiProcessorCount * 8;
  if (max_blocks < 1) max_blocks = 1;
  long long elems_per_block_iter = static_cast<long long>(block_dim) * UNROLL;
  int suggested = static_cast<int>((n + elems_per_block_iter - 1) / elems_per_block_iter);
  if (suggested < 1) suggested = 1;
  int grid_dim = suggested < max_blocks ? suggested : max_blocks;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<UNROLL><<<grid_dim, block_dim>>>(input, output, n);
  }
}
