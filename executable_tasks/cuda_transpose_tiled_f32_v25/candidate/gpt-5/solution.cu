#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ __launch_bounds__(kTileDim * kBlockRows, 2)
void transpose_tiled_kernel(const float* __restrict__ input,
                            float* __restrict__ output,
                            int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];

  int x = blockIdx.x * kTileDim + threadIdx.x;
  int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from global memory to shared memory
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      tile[threadIdx.y + j][threadIdx.x] = input[yj * width + x];
    }
  }

  __syncthreads();

  // Transpose coordinates for writing
  int x2 = blockIdx.y * kTileDim + threadIdx.x;  // becomes column in output
  int y2 = blockIdx.x * kTileDim + threadIdx.y;  // becomes row in output

  // Store transposed tile to global memory from shared memory
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int y2j = y2 + j;
    if (x2 < height && y2j < width) {
      output[x2 * height + y2j] = tile[threadIdx.x][threadIdx.y + j];
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  // Prefer shared memory for this kernel
  cudaFuncSetCacheConfig(transpose_tiled_kernel, cudaFuncCachePreferShared);

  const dim3 block(kTileDim, kBlockRows, 1);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim,
                  1);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
