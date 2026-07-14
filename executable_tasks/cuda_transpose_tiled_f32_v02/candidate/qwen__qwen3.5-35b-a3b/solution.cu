#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 16;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
  __shared__ float tile[kTileDim][kTileDim];

  const int x = blockIdx.x * kTileDim + threadIdx.x;
  const int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load input tile into shared memory
  if (x < width && y < height) {
    tile[threadIdx.y][threadIdx.x] = input[y * width + x];
  }
  __syncthreads();

  // Transpose and write to output
  if (x < height && y < width) {
    output[x * height + y] = tile[threadIdx.x][threadIdx.y];
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kTileDim);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
  }
}