#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

// Generic tiled transpose with shared memory and padding to avoid bank conflicts
__global__ void transpose_tiled_generic(const float* __restrict__ input,
                                        float* __restrict__ output,
                                        int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x = blockIdx.x * kTileDim + threadIdx.x;
  int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from input (coalesced reads)
#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Transpose block offset for writing
  int ox = blockIdx.y * kTileDim + threadIdx.x;
  int oy = blockIdx.x * kTileDim + threadIdx.y;

  // Store transposed tile to output (coalesced writes)
#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int oyj = oy + j;
    if (ox < height && oyj < width) {
      output[oyj * height + ox] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

// Fast-path tiled transpose without bounds checks (use only when width and height are multiples of kTileDim)
__global__ void transpose_tiled_nobounds(const float* __restrict__ input,
                                         float* __restrict__ output,
                                         int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x = blockIdx.x * kTileDim + threadIdx.x;
  int y = blockIdx.y * kTileDim + threadIdx.y;

#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
  }

  __syncthreads();

  int ox = blockIdx.y * kTileDim + threadIdx.x;
  int oy = blockIdx.x * kTileDim + threadIdx.y;

#pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    output[(oy + j) * height + ox] = tile[threadIdx.x][threadIdx.y + j];
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

  // Prefer shared memory for this kernel pattern
  cudaFuncSetCacheConfig(transpose_tiled_generic, cudaFuncCachePreferShared);
  cudaFuncSetCacheConfig(transpose_tiled_nobounds, cudaFuncCachePreferShared);

  const bool full_tiles = (width % kTileDim == 0) && (height % kTileDim == 0);

  for (int iter = 0; iter < iters; ++iter) {
    if (full_tiles) {
      transpose_tiled_nobounds<<<grid, block>>>(input, output, width, height);
    } else {
      transpose_tiled_generic<<<grid, block>>>(input, output, width, height);
    }
  }
}
