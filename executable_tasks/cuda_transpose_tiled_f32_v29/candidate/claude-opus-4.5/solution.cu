#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                        float* __restrict__ output,
                                        int width, int height) {
  // Shared memory tile with padding to avoid bank conflicts
  __shared__ float tile[kTileDim][kTileDim + 1];

  // Input coordinates
  int xIn = blockIdx.x * kTileDim + threadIdx.x;
  int yIn = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from input (coalesced reads)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yIdx = yIn + j;
    if (xIn < width && yIdx < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yIdx * width + xIn];
    }
  }

  __syncthreads();

  // Output coordinates (transposed block position)
  int xOut = blockIdx.y * kTileDim + threadIdx.x;
  int yOut = blockIdx.x * kTileDim + threadIdx.y;

  // Write transposed tile to output (coalesced writes)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yIdx = yOut + j;
    if (xOut < height && yIdx < width) {
      output[yIdx * height + xOut] = tile[threadIdx.x][threadIdx.y + j];
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
