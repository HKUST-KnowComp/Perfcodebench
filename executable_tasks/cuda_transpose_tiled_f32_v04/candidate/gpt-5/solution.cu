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

  bool valid_x = (x < width);

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (valid_x && yj < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yj * width + x];
    }
  }

  __syncthreads();

  int x2 = blockIdx.y * kTileDim + threadIdx.x;  // column index in output (0..height-1)
  int y2 = blockIdx.x * kTileDim + threadIdx.y;  // row index in output (0..width-1)

  bool valid_x2 = (x2 < height);

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int y2j = y2 + j;
    if (valid_x2 && y2j < width) {
      output[y2j * height + x2] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
