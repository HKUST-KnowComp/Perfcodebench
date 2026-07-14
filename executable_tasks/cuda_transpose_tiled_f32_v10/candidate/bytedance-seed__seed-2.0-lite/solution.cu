#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
  // Shared memory tile with +1 padding to eliminate shared memory bank conflicts
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  // Input tile base coordinates
  const int x_in = blockIdx.x * kTileDim + tx;
  const int y_in_base = blockIdx.y * kTileDim;

  // Load input tile from global memory to shared memory
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y_in = y_in_base + ty + j;
    if (x_in < width && y_in < height) {
      tile[ty + j][tx] = input[y_in * width + x_in];
    }
  }

  // Synchronize all threads to ensure tile is fully loaded before writing
  __syncthreads();

  // Output tile coordinates (transposed block position)
  const int x_out = blockIdx.y * kTileDim + tx;
  const int y_out_base = blockIdx.x * kTileDim;

  // Write transposed tile from shared memory to global memory
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y_out = y_out_base + ty + j;
    if (x_out < height && y_out < width) {
      output[y_out * height + x_out] = tile[tx][ty + j];
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