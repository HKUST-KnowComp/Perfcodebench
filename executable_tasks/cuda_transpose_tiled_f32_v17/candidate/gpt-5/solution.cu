#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int x = blockIdx.x * kTileDim + threadIdx.x;
  const int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from input into shared memory (coalesced reads)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Compute coordinates for writing the transposed tile
  const int new_x = blockIdx.y * kTileDim + threadIdx.x;  // becomes row index in output
  const int new_y = blockIdx.x * kTileDim + threadIdx.y;  // becomes column base in output

  // Store transposed tile from shared memory to output (coalesced writes)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int nyj = new_y + j;
    if (new_x < height && nyj < width) {
      // output has dimensions [width x height] -> widthOut = height
      output[nyj * height + new_x] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (width <= 0 || height <= 0 || iters <= 0) return;

  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim);

  // Favor shared memory for this kernel (small smem usage but aligns with tiled transpose)
  cudaFuncSetCacheConfig(transpose_tiled_kernel, cudaFuncCachePreferShared);

  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
