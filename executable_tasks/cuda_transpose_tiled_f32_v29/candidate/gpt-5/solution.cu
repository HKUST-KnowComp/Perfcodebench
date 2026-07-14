#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  // Pad second dimension to avoid shared memory bank conflicts during transpose
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int x = blockIdx.x * kTileDim + threadIdx.x;
  const int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load a 32x32 tile from input into shared memory (coalesced reads)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int yj = y + j;
    if (x < width && yj < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Transposed coordinates for writing from shared memory to output
  const int xo = blockIdx.y * kTileDim + threadIdx.x; // output column (ranges over height)
  const int yo = blockIdx.x * kTileDim + threadIdx.y; // output row (ranges over width)

  // Store transposed tile to output (coalesced writes)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int yoj = yo + j;
    if (xo < height && yoj < width) {
      output[yoj * height + xo] = tile[threadIdx.x][threadIdx.y + j];
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
