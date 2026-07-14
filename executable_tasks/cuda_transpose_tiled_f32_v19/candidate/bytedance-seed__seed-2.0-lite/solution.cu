#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
  __shared__ float s_tile[32][33]; // 33-wide pitch avoids shared memory bank conflicts
  const int x = blockIdx.x * kTileDim + threadIdx.x;
  const int y_base = blockIdx.y * kTileDim + threadIdx.y;

  // Load input tile into shared memory (coalesced global reads)
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y = y_base + j;
    if (x < width && y < height) {
      s_tile[threadIdx.y + j][threadIdx.x] = input[y * width + x];
    }
  }

  // Synchronize to ensure full tile is loaded before writes
  __syncthreads();

  // Write transposed tile to output (coalesced global writes)
  const int out_x = blockIdx.y * kTileDim + threadIdx.x;
  const int out_y_base = blockIdx.x * kTileDim + threadIdx.y;
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int out_y = out_y_base + j;
    if (out_x < height && out_y < width) {
      output[out_y * height + out_x] = s_tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}