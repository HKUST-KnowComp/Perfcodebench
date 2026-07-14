#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

// Tiled shared-memory transpose with bank-conflict padding and coalesced read/write.
__global__ __launch_bounds__(kTileDim * kBlockRows)
void transpose_tiled_kernel(const float* __restrict__ input,
                            float* __restrict__ output,
                            int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int x = blockIdx.x * kTileDim + threadIdx.x;  // column in input
  const int y = blockIdx.y * kTileDim + threadIdx.y;  // row in input

  // Load tile from input -> shared memory (coalesced reads)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yy = y + j;
    if (x < width && yy < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yy * width + x];
    }
  }

  __syncthreads();

  // Transposed coordinates for write-back
  const int tx = blockIdx.y * kTileDim + threadIdx.x; // row index in output (equals input column)
  const int ty_base = blockIdx.x * kTileDim + threadIdx.y; // column index base in output (equals input row)

  // Store transposed tile from shared memory -> output (coalesced writes)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int ty = ty_base + j;
    if (tx < height && ty < width) {
      output[tx * height + ty] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (width <= 0 || height <= 0 || iters <= 0) return;
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
