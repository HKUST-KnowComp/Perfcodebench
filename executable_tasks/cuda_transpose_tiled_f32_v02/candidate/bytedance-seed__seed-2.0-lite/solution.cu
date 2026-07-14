#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
  __shared__ float s_tile[kTileDim][kTileDim + 1]; // +1 padding eliminates shared memory bank conflicts

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int x = blockIdx.x * kTileDim + tx;
  const int y_base = blockIdx.y * kTileDim;

  // Coalesced load of input tile into shared memory
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y = y_base + ty + j;
    if (x < width && y < height) {
      s_tile[ty + j][tx] = input[y * width + x];
    }
  }

  __syncthreads(); // Wait for all tile loads to complete before writing transposed data

  // Coalesced write of transposed tile to global output
  const int out_x = blockIdx.y * kTileDim + tx;
  const int out_y_base = blockIdx.x * kTileDim;
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int out_y = out_y_base + ty + j;
    if (out_x < height && out_y < width) {
      output[out_y * height + out_x] = s_tile[tx][ty + j];
    }
  }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
  }
}