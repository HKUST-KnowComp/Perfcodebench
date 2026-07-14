#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width,
                                       int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int x_in = blockIdx.x * kTileDim + threadIdx.x;
  const int y_in = blockIdx.y * kTileDim + threadIdx.y;

#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y = y_in + j;
    if (x_in < width && y < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[y * width + x_in];
    }
  }

  __syncthreads();

  const int x_out = blockIdx.y * kTileDim + threadIdx.x;
  const int y_out = blockIdx.x * kTileDim + threadIdx.y;

#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y = y_out + j;
    if (x_out < height && y < width) {
      output[y * height + x_out] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (iters <= 0 || width <= 0 || height <= 0) {
    return;
  }

  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim);

  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
