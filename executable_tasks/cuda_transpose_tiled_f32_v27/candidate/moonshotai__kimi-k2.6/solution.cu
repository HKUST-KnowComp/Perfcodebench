#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__launch_bounds__(kTileDim * kBlockRows)
__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  // Coalesced load from input into shared memory
  int x = blockIdx.x * kTileDim + threadIdx.x;
  int y = blockIdx.y * kTileDim + threadIdx.y;

  if (x < width) {
    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
      if (y + j < height) {
        tile[threadIdx.y + j][threadIdx.x] =
            __ldg(&input[static_cast<size_t>(y + j) * width + x]);
      }
    }
  }

  __syncthreads();

  // Coalesced write from shared memory to output (transposed)
  x = blockIdx.y * kTileDim + threadIdx.x;
  y = blockIdx.x * kTileDim + threadIdx.y;

  if (x < height) {
    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
      if (y + j < width) {
        output[static_cast<size_t>(y + j) * height + x] =
            tile[threadIdx.x][threadIdx.y + j];
      }
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
