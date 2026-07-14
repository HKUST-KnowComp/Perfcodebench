#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  __shared__ float tileA[TILE_SIZE][TILE_SIZE];
  __shared__ float tileB[TILE_SIZE][TILE_SIZE];

  int bx = blockIdx.x;
  int by = blockIdx.y;
  int tx = threadIdx.x;
  int ty = threadIdx.y;

  int row = by * TILE_SIZE + ty;
  int col = bx * TILE_SIZE + tx;

  float sum = 0.0f;

  for (int tile = 0; tile < (n + TILE_SIZE - 1) / TILE_SIZE; ++tile) {
    // Cooperative loading of tile of A
    int a_row = row;
    int a_col = tile * TILE_SIZE + tx;
    if (a_row < n && a_col < n)
      tileA[ty][tx] = a[a_row * n + a_col];
    else
      tileA[ty][tx] = 0.0f;

    // Cooperative loading of tile of B
    int b_row = tile * TILE_SIZE + ty;
    int b_col = col;
    if (b_row < n && b_col < n)
      tileB[ty][tx] = b[b_row * n + b_col];
    else
      tileB[ty][tx] = 0.0f;

    __syncthreads();

    // Compute partial product for this tile
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += tileA[ty][k] * tileB[k][tx];
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
