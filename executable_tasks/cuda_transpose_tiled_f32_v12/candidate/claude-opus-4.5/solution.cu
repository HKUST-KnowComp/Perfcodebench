#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int xIndex = blockIdx.x * kTileDim + threadIdx.x;
  int yIndex = blockIdx.y * kTileDim + threadIdx.y;

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int srcY = yIndex + j;
    if (xIndex < width && srcY < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[srcY * width + xIndex];
    }
  }

  __syncthreads();

  int xIndexOut = blockIdx.y * kTileDim + threadIdx.x;
  int yIndexOut = blockIdx.x * kTileDim + threadIdx.y;

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int dstY = yIndexOut + j;
    if (xIndexOut < height && dstY < width) {
      output[dstY * height + xIndexOut] = tile[threadIdx.x][threadIdx.y + j];
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
