#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* input, float* output,
                                       int width, int height) {
  // Shared memory with padding to avoid bank conflicts on transposed read
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x_in = blockIdx.x * kTileDim + threadIdx.x;
  int y_in = blockIdx.y * kTileDim + threadIdx.y;

  // Coalesced read from global memory
  if (x_in < width && y_in < height) {
    tile[threadIdx.y][threadIdx.x] = input[y_in * width + x_in];
  } else {
    tile[threadIdx.y][threadIdx.x] = 0.0f;
  }

  __syncthreads();

  // Transposed coordinates for output
  int x_out = blockIdx.x * kTileDim + threadIdx.y;  // row in output (was column in input)
  int y_out = blockIdx.y * kTileDim + threadIdx.x;  // column in output (was row in input)

  // Coalesced write to global memory (padding avoids bank conflicts in the read)
  if (x_out < width && y_out < height) {
    output[x_out * height + y_out] = tile[threadIdx.x][threadIdx.y];
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height,
                   int iters) {
  dim3 block(kTileDim, kTileDim);
  dim3 grid((width + kTileDim - 1) / kTileDim,
            (height + kTileDim - 1) / kTileDim);

  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
