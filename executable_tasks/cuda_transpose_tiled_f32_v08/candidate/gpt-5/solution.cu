#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

// Shared-memory tiled transpose with bank-conflict-free padding and diagonal block reordering
__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  // Diagonal reordering of blocks to reduce partition camping
  int bx, by;
  if (gridDim.x == gridDim.y) {
    by = blockIdx.y;
    bx = (blockIdx.x + blockIdx.y) % gridDim.x;
  } else {
    int bid = blockIdx.x + blockIdx.y * gridDim.x;
    by = bid % gridDim.y;
    bx = ((bid / gridDim.y) + by) % gridDim.x;
  }

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  int x = bx * kTileDim + tx;
  int y = by * kTileDim + ty;

  // Global read, coalesced, into shared memory
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      tile[ty + j][tx] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Transposed coordinates for write-out
  int x2 = by * kTileDim + tx;  // becomes column index in output (bound by height)
  int y2 = bx * kTileDim + ty;  // becomes row index in output (bound by width)

  // Global write, coalesced, from shared memory (transposed)
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int y2j = y2 + j;
    if (x2 < height && y2j < width) {
      output[y2j * height + x2] = tile[tx][ty + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (width <= 0 || height <= 0 || iters <= 0) return;

  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

  cudaFuncSetCacheConfig(transpose_tiled_kernel, cudaFuncCachePreferShared);

  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
