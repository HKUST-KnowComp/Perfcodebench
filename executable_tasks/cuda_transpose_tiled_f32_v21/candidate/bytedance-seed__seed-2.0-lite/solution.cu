#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
  // 32x32 tile with 1-element padding to eliminate shared memory bank conflicts
  __shared__ float tile[32][33];
  const int bx = blockIdx.x;
  const int by = blockIdx.y;
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  // Load tile from input with coalesced global memory loads
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y = by * kTileDim + ty + j;
    const int x = bx * kTileDim + tx;
    if (x < width && y < height) {
      tile[ty + j][tx] = input[y * width + x];
    }
  }

  // Synchronize all threads to ensure tile is fully loaded
  __syncthreads();

  // Write transposed tile to output with coalesced global memory stores
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y_src = ty + j;
    const int x_src = tx;
    const int x_dst = bx * kTileDim + y_src;
    const int y_dst = by * kTileDim + x_src;
    if (x_dst < width && y_dst < height) {
      output[x_dst * height + y_dst] = tile[y_src][x_src];
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