#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  __shared__ float a_tile[TILE_SIZE][TILE_SIZE];
  __shared__ float b_tile[TILE_SIZE][TILE_SIZE];

  const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  const int col = blockIdx.x * TILE_SIZE + threadIdx.x;
  float sum = 0.0f;
  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int t = 0; t < num_tiles; ++t) {
    // Load A tile into shared memory with bounds checking
    const int a_col = t * TILE_SIZE + threadIdx.x;
    if (row < n && a_col < n) {
      a_tile[threadIdx.y][threadIdx.x] = a[row * n + a_col];
    } else {
      a_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    // Load B tile into shared memory with bounds checking
    const int b_row = t * TILE_SIZE + threadIdx.y;
    if (b_row < n && col < n) {
      b_tile[threadIdx.y][threadIdx.x] = b[b_row * n + col];
    } else {
      b_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    // Synchronize to ensure all tiles are loaded
    __syncthreads();

    // Compute partial sum for this tile
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += a_tile[threadIdx.y][k] * b_tile[k][threadIdx.x];
    }

    // Synchronize before loading next tile to avoid race conditions
    __syncthreads();
  }

  // Write final result to global memory
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE_SIZE, TILE_SIZE);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}