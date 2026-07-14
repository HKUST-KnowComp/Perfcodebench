#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

// Tiled shared-memory transpose with bank-conflict avoidance and coalesced reads/writes
__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x = blockIdx.x * kTileDim + threadIdx.x;  // column index in input
  int y = blockIdx.y * kTileDim + threadIdx.y;  // row index in input

  // Load tile from input to shared memory (coalesced reads)
#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yy = y + j;
    if (x < width && yy < height) {
#if __CUDA_ARCH__ >= 350
      tile[threadIdx.y + j][threadIdx.x] = __ldg(&input[yy * width + x]);
#else
      tile[threadIdx.y + j][threadIdx.x] = input[yy * width + x];
#endif
    }
  }

  __syncthreads();

  // Transposed coordinates for output
  int ox = blockIdx.y * kTileDim + threadIdx.x;  // column index in output
  int oy = blockIdx.x * kTileDim + threadIdx.y;  // row index in output

  // Store transposed tile to output (coalesced writes)
#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int oyy = oy + j;
    if (ox < height && oyy < width) {
      // output is [width x height] with row-major layout: row_out * height + col_out
      output[oyy * height + ox] = tile[threadIdx.x][threadIdx.y + j];
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
