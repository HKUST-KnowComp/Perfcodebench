#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ __launch_bounds__(kTileDim * kBlockRows, 2)
void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x = blockIdx.x * kTileDim + threadIdx.x;
  int y_base = blockIdx.y * kTileDim + threadIdx.y;

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int y = y_base + j;
    if (x < width && y < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[y * width + x];
    }
  }
  __syncthreads();

  int x_t = blockIdx.y * kTileDim + threadIdx.x; // column index in transposed matrix (range: [0, height))
  int y_t_base = blockIdx.x * kTileDim + threadIdx.y; // row index in transposed matrix (range: [0, width))

  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int y_t = y_t_base + j;
    if (x_t < height && y_t < width) {
      output[y_t * height + x_t] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (width <= 0 || height <= 0 || iters <= 0) return;
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
