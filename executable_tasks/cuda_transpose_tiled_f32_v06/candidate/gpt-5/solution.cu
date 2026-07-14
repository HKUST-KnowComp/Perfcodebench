#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x = blockIdx.x * kTileDim + threadIdx.x;
  int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from global memory to shared memory (coalesced reads)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Transposed coordinates for writing
  int out_x = blockIdx.y * kTileDim + threadIdx.x;
  int out_y_base = blockIdx.x * kTileDim + threadIdx.y;

  // Write transposed tile to global memory (coalesced writes)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int out_y = out_y_base + j;
    if (out_x < height && out_y < width) {
      output[out_x * height + out_y] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (width <= 0 || height <= 0 || iters <= 0) return;
  const dim3 block(kTileDim, kBlockRows, 1);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim, 1);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
