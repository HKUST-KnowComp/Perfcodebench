#include "interface.h"
#include <cuda_runtime.h>

namespace {
#define TILE_SIZE 32

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  const int col = blockIdx.x * TILE_SIZE + threadIdx.x;

  __shared__ float As[TILE_SIZE][TILE_SIZE];
  __shared__ float Bs[TILE_SIZE][TILE_SIZE];
  float sum = 0.0f;

  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;
  for (int k_tile = 0; k_tile < num_tiles; ++k_tile) {
    // Load tile of A into shared memory
    const int a_col = k_tile * TILE_SIZE + threadIdx.x;
    if (row < n && a_col < n) {
      As[threadIdx.y][threadIdx.x] = a[row * n + a_col];
    } else {
      As[threadIdx.y][threadIdx.x] = 0.0f;
    }

    // Load tile of B into shared memory
    const int b_row = k_tile * TILE_SIZE + threadIdx.y;
    if (b_row < n && col < n) {
      Bs[threadIdx.y][threadIdx.x] = b[b_row * n + col];
    } else {
      Bs[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Accumulate partial sum for this tile
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
    }

    __syncthreads();
  }

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