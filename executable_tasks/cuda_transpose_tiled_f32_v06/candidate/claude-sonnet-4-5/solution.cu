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
    const int row = y + j;
    if (x < width && row < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[row * width + x];
    }
  }
  
  __syncthreads();
  
  const int tx = blockIdx.y * kTileDim + threadIdx.x;
  const int ty = blockIdx.x * kTileDim + threadIdx.y;
  
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int col = ty + j;
    if (tx < height && col < width) {
      output[col * height + tx] = tile[threadIdx.x][threadIdx.y + j];
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
