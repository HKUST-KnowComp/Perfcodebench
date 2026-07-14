#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
  // Shared memory tile with +1 padding to eliminate shared memory bank conflicts
  __shared__ float tile[kTileDim][kTileDim + 1];

  const int block_x = blockIdx.x * kTileDim;
  const int block_y = blockIdx.y * kTileDim;
  const int thread_x = threadIdx.x;
  const int thread_y = threadIdx.y;

  // Load input tile into shared memory with coalesced global reads
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int in_y = block_y + thread_y + j;
    const int in_x = block_x + thread_x;
    if (in_x < width && in_y < height) {
      tile[thread_y + j][thread_x] = input[in_y * width + in_x];
    }
  }

  __syncthreads(); // Wait for all threads to finish loading tile data

  // Write transposed tile to output with coalesced global writes
  const int out_block_x = blockIdx.y * kTileDim;
  const int out_block_y = blockIdx.x * kTileDim;
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    const int out_y = out_block_y + thread_y + j;
    const int out_x = out_block_x + thread_x;
    if (out_x < height && out_y < width) {
      output[out_y * height + out_x] = tile[thread_x][thread_y + j];
    }
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