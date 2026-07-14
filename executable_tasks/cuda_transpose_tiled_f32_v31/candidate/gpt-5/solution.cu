#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int x = blockIdx.x * kTileDim + tx;
  const int y = blockIdx.y * kTileDim + ty;

  int in_idx = y * width + x;
  const int in_stride = kBlockRows * width;

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (x < width && (y + j) < height) {
      tile[ty + j][tx] = input[in_idx];
    }
    in_idx += in_stride;
  }

  __syncthreads();

  const int outx = blockIdx.y * kTileDim + tx;
  const int outy = blockIdx.x * kTileDim + ty;

  int out_idx = outy * height + outx;
  const int out_stride = kBlockRows * height;

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (outx < height && (outy + j) < width) {
      output[out_idx] = tile[tx][ty + j];
    }
    out_idx += out_stride;
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
