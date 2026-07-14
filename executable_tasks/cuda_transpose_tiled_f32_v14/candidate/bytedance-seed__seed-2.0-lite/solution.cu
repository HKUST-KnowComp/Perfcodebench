#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
  // Shared memory tile padded by 1 element to avoid 32-way bank conflicts
  __shared__ float s_tile[kTileDim][kTileDim + 1];
  
  const int block_x = blockIdx.x * kTileDim;
  const int block_y = blockIdx.y * kTileDim;
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  // Load input tile into shared memory (coalesced global reads)
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int gx = block_x + tx;
    const int gy = block_y + ty + j;
    if (gx < width && gy < height) {
      s_tile[ty + j][tx] = input[gy * width + gx];
    }
  }

  // Synchronize to ensure entire tile is loaded before transposed write
  __syncthreads();

  // Write transposed tile to global output (coalesced global writes)
  const int out_block_x = block_y;
  const int out_block_y = block_x;
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int gx = out_block_x + tx;
    const int gy = out_block_y + ty + j;
    if (gx < height && gy < width) {
      output[gy * height + gx] = s_tile[tx][ty + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}