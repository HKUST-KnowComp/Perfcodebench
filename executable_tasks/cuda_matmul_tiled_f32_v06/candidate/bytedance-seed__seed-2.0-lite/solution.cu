#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_WIDTH = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float s_a[TILE_WIDTH][TILE_WIDTH];
  __shared__ float s_b[TILE_WIDTH][TILE_WIDTH];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * TILE_WIDTH + tx;
  const int row = blockIdx.y * TILE_WIDTH + ty;
  float sum = 0.0f;

  const int num_tiles = (n + TILE_WIDTH - 1) / TILE_WIDTH;
  for (int k_tile = 0; k_tile < num_tiles; ++k_tile) {
    const int a_col = k_tile * TILE_WIDTH + tx;
    const int b_row = k_tile * TILE_WIDTH + ty;

    s_a[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
    s_b[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

    __syncthreads();

    for (int k = 0; k < TILE_WIDTH; ++k) {
      sum += s_a[ty][k] * s_b[k][tx];
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE_WIDTH, TILE_WIDTH);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}