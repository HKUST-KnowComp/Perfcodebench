#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle instructions.
__device__ __forceinline__ int warp_reduce_sum(int v) {
  unsigned mask = 0xffffffffu;
  v += __shfl_down_sync(mask, v, 16);
  v += __shfl_down_sync(mask, v, 8);
  v += __shfl_down_sync(mask, v, 4);
  v += __shfl_down_sync(mask, v, 2);
  v += __shfl_down_sync(mask, v, 1);
  return v;
}

// First stage: each block reduces a grid-stride chunk into one partial sum.
__global__ void reduce_stage1_kernel(const int* __restrict__ input,
                                     int* __restrict__ partials,
                                     int n) {
  int sum = 0;
  int idx = static_cast<int>(blockIdx.x) * blockDim.x + threadIdx.x;
  int stride = static_cast<int>(gridDim.x) * blockDim.x;

  // Grid-stride loop with 4x unrolling to improve memory throughput.
  int i = idx;
  int limit4 = n - stride * 3;
  for (; i < limit4; i += stride * 4) {
    sum += input[i];
    sum += input[i + stride];
    sum += input[i + stride * 2];
    sum += input[i + stride * 3];
  }
  for (; i < n; i += stride) {
    sum += input[i];
  }

  // Block reduction via warp reductions + shared memory for warp leaders.
  __shared__ int warp_sums[32];
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;

  sum = warp_reduce_sum(sum);
  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  int block_sum = 0;
  if (warp == 0) {
    int num_warps = blockDim.x >> 5;
    block_sum = (lane < num_warps) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) partials[blockIdx.x] = block_sum;
  }
}

// Final stage: one block reduces the partial sums to a single output.
__global__ void reduce_stage2_kernel(const int* __restrict__ partials,
                                     int* __restrict__ output,
                                     int n) {
  int sum = 0;
  for (int i = threadIdx.x; i < n; i += blockDim.x) {
    sum += partials[i];
  }

  __shared__ int warp_sums[32];
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;

  sum = warp_reduce_sum(sum);
  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int num_warps = blockDim.x >> 5;
    int block_sum = (lane < num_warps) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) *output = block_sum;
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
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

  constexpr int block_size = 256;
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;

  // Enough blocks to saturate the GPU without creating excessive partials.
  int grid_size = sm_count * 8;
  int max_blocks_for_n = (n + block_size - 1) / block_size;
  if (grid_size > max_blocks_for_n) grid_size = max_blocks_for_n;
  if (grid_size < 1) grid_size = 1;

  int* partials = nullptr;
  cudaMalloc(&partials, static_cast<size_t>(grid_size) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_stage1_kernel<<<grid_size, block_size>>>(input, partials, n);
    reduce_stage2_kernel<<<1, block_size>>>(partials, output, grid_size);
  }

  cudaFree(partials);
}
