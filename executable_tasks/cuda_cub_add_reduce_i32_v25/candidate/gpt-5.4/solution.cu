#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ int warp_reduce_sum(int v) {
  for (int offset = 16; offset > 0; offset >>= 1) {
    v += __shfl_down_sync(0xffffffff, v, offset);
  }
  return v;
}

__global__ void reduce_blocks_kernel(const int* __restrict__ input,
                                     int* __restrict__ partial,
                                     int n) {
  int sum = 0;
  int idx = blockIdx.x * blockDim.x * 2 + threadIdx.x;
  int stride = blockDim.x * 2 * gridDim.x;

  while (idx < n) {
    sum += input[idx];
    int idx2 = idx + blockDim.x;
    if (idx2 < n) sum += input[idx2];
    idx += stride;
  }

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[32];
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int block_sum = (lane < (blockDim.x + 31) / 32) ? warp_sums[lane] : 0;
    block_sum = warp_reduce_sum(block_sum);
    if (lane == 0) partial[blockIdx.x] = block_sum;
  }
}

__global__ void finalize_kernel(const int* __restrict__ partial,
                                int* __restrict__ output,
                                int n) {
  int sum = 0;
  for (int idx = threadIdx.x; idx < n; idx += blockDim.x) sum += partial[idx];

  sum = warp_reduce_sum(sum);

  __shared__ int warp_sums[32];
  int lane = threadIdx.x & 31;
  int warp = threadIdx.x >> 5;

  if (lane == 0) warp_sums[warp] = sum;
  __syncthreads();

  if (warp == 0) {
    int total = (lane < (blockDim.x + 31) / 32) ? warp_sums[lane] : 0;
    total = warp_reduce_sum(total);
    if (lane == 0) *output = total;
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

  const int block_size = 256;
  int max_blocks = prop.multiProcessorCount * 8;
  int grid_size = (n + block_size * 2 - 1) / (block_size * 2);
  if (grid_size < 1) grid_size = 1;
  if (grid_size > max_blocks) grid_size = max_blocks;

  int* partial = nullptr;
  cudaMalloc(&partial, static_cast<size_t>(grid_size) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_blocks_kernel<<<grid_size, block_size>>>(input, partial, n);
    finalize_kernel<<<1, 256>>>(partial, output, grid_size);
  }

  cudaFree(partial);
}
