#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int in_col = blockIdx.x * kTileDim + threadIdx.x;
  int in_row = blockIdx.y * kTileDim + threadIdx.y;

  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (in_col < width && in_row + j < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[(in_row + j) * width + in_col];
    }
  }
  __syncthreads();

  int out_row = blockIdx.x * kTileDim + threadIdx.y;
  int out_col = blockIdx.y * kTileDim + threadIdx.x;

  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (out_row + j < width && out_col < height) {
      output[(out_row + j) * height + out_col] = tile[threadIdx.x][threadIdx.y + j];
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
