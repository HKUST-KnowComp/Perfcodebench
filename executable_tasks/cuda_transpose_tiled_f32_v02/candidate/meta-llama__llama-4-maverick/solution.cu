#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];
  const int x = blockIdx.x * kTileDim + threadIdx.x;
  const int y = blockIdx.y * kTileDim + threadIdx.y;
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (x < width && y + j < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
    }
  }
  __syncthreads();
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (blockIdx.y * kTileDim + threadIdx.x < height && blockIdx.x * kTileDim + threadIdx.y + j < width) {
      output[(blockIdx.x * kTileDim + threadIdx.y + j) * height + blockIdx.y * kTileDim + threadIdx.x] = tile[threadIdx.x][threadIdx.y + j];
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