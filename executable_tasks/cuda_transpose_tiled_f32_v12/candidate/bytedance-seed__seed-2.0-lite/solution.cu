#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
  // Shared tile with 1-column padding to avoid shared memory bank conflicts
  __shared__ float s_tile[32][33];

  const int block_x = blockIdx.x * kTileDim;
  const int block_y = blockIdx.y * kTileDim;
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  // Load input tile into shared memory with coalesced global reads
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int x = block_x + tx;
    const int y = block_y + ty + j;
    if (x < width && y < height) {
      s_tile[ty + j][tx] = input[y * width + x];
    }
  }

  // Synchronize all threads before writing transposed tile
  __syncthreads();

  // Write transposed tile to output with coalesced global writes
  const int out_block_x = block_y;
  const int out_block_y = block_x;
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int x = out_block_x + tx;
    const int y = out_block_y + ty + j;
    if (x < height && y < width) {
      output[y * height + x] = s_tile[tx][ty + j];
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