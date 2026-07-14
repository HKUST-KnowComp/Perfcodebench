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

  // Input coordinates (coalesced read)
  int x_in = blockIdx.x * kTileDim + threadIdx.x;
  int y_in = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from input (coalesced global reads)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (x_in < width && (y_in + j) < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[(y_in + j) * width + x_in];
    }
  }

  __syncthreads();

  // Output coordinates (transposed block position)
  int x_out = blockIdx.y * kTileDim + threadIdx.x;
  int y_out = blockIdx.x * kTileDim + threadIdx.y;

  // Store tile to output (coalesced global writes)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    if (x_out < height && (y_out + j) < width) {
      output[(y_out + j) * height + x_out] = tile[threadIdx.x][threadIdx.y + j];
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
