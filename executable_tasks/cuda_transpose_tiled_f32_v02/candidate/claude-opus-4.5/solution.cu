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
    if (xIndex < width && (yIndex + j) < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[(yIndex + j) * width + xIndex];
    }
  }

  __syncthreads();

  xIndex = blockIdx.y * kTileDim + threadIdx.x;
  yIndex = blockIdx.x * kTileDim + threadIdx.y;

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (xIndex < height && (yIndex + j) < width) {
      output[(yIndex + j) * height + xIndex] = tile[threadIdx.x][threadIdx.y + j];
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
