#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ __launch_bounds__(kTileDim * kBlockRows)
void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1]; // +1 to avoid shared memory bank conflicts

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int x = blockIdx.x * kTileDim + tx; // input column index
  const int y = blockIdx.y * kTileDim + ty; // input row index

  // Load phase: coalesced reads from input into shared memory
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      tile[ty + j][tx] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Store phase: write transposed tile to output with coalesced accesses
  const int out_row = blockIdx.x * kTileDim + tx;       // output row index (was input column)
  const int out_col_base = blockIdx.y * kTileDim + ty;  // output column base (was input row)

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int out_col = out_col_base + j;
    if (out_row < width && out_col < height) {
      output[out_row * height + out_col] = tile[tx][ty + j];
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
