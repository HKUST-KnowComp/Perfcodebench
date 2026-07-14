#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
  __shared__ float tile[kBlockRows][kTileDim + 1]; // +1 eliminates shared memory bank conflicts
  const int block_x = blockIdx.x * kTileDim;
  const int block_y_base = blockIdx.y * kTileDim;
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int y = block_y_base + j + ty;
    const int x = block_x + tx;
    // Load input tile to shared memory with coalesced global reads
    if (x < width && y < height) {
      tile[ty][tx] = input[y * width + x];
    }
    // Synchronize to ensure all tile elements are loaded before use
    __syncthreads();
    // Write transposed tile to global memory with coalesced writes
    const int out_y = block_x + ty;
    const int out_x = block_y_base + j + tx;
    if (out_y < width && out_x < height) {
      output[out_y * height + out_x] = tile[tx][ty];
    }
    // Synchronize to avoid overwriting shared memory before all writes complete
    __syncthreads();
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
  for (int iter = 0; iter < iters; ++iter) {
    transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
  }
}