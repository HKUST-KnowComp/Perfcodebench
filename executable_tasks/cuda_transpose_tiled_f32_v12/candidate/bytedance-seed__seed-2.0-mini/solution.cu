#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
  // Padded shared memory to eliminate bank conflicts
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int block_start_col = blockIdx.x * kTileDim;
  const int block_start_row = blockIdx.y * kTileDim;

  // Load input tile into shared memory with coalesced global loads
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int tile_row = ty + j;
    const int global_row = block_start_row + tile_row;
    const int global_col = block_start_col + tx;

    if (global_col < width && global_row < height) {
      tile[tile_row][tx] = input[global_row * width + global_col];
    }
  }

  __syncthreads();

  // Write transposed tile to global memory with coalesced global stores
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int tile_row = ty + j;
    const int output_global_col = block_start_row + tile_row;
    const int output_global_row = block_start_col + tx;

    if (output_global_row < width && output_global_col < height) {
      output[output_global_row * height + output_global_col] = tile[tx][tile_row];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
  }
}