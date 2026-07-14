#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* __restrict__ input,
                                           float* __restrict__ output,
                                           int width, int height) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int tile_x = blockIdx.x * kTileDim;
  const int tile_y = blockIdx.y * kTileDim;

  __shared__ float smem[kTileDim][kTileDim + 1];

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int input_row = tile_y + ty + j;
    const int input_col = tile_x + tx;

    if (input_col < width && input_row < height) {
      smem[ty + j][tx] = input[input_row * width + input_col];
    } else {
      smem[ty + j][tx] = 0.0f;
    }
  }

  __syncthreads();

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int output_row = blockIdx.x * kTileDim + tx;
    const int output_col = blockIdx.y * kTileDim + ty + j;

    if (output_row < height && output_col < width) {
      output[output_col * height + output_row] = smem[tx][ty + j];
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