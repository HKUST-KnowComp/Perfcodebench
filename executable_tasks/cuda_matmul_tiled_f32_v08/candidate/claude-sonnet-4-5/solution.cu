#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int BLOCK_SIZE = 16;
constexpr int THREAD_TILE = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float as[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float bs[TILE_SIZE][TILE_SIZE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int bx = blockIdx.x;
  const int by = blockIdx.y;

  const int row_base = by * TILE_SIZE + ty * 2;
  const int col_base = bx * TILE_SIZE + tx * 2;

  float acc[THREAD_TILE][THREAD_TILE] = {0.0f};

  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int tile = 0; tile < num_tiles; ++tile) {
    const int a_col = tile * TILE_SIZE + tx * 2;
    const int b_row = tile * TILE_SIZE + ty * 2;

    #pragma unroll
    for (int i = 0; i < 2; ++i) {
      #pragma unroll
      for (int j = 0; j < 2; ++j) {
        int row = row_base + i;
        int col = a_col + j;
        as[ty * 2 + i][tx * 2 + j] = (row < n && col < n) ? a[row * n + col] : 0.0f;
      }
    }

    #pragma unroll
    for (int i = 0; i < 2; ++i) {
      #pragma unroll
      for (int j = 0; j < 2; ++j) {
        int row = b_row + i;
        int col = col_base + j;
        bs[ty * 2 + i][tx * 2 + j] = (row < n && col < n) ? b[row * n + col] : 0.0f;
      }
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      #pragma unroll
      for (int i = 0; i < THREAD_TILE; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE; ++j) {
          acc[i][j] += as[ty * 2 + i / 4][k] * bs[k][tx * 2 + j / 4];
        }
      }
    }

    __syncthreads();
  }

  #pragma unroll
  for (int i = 0; i < THREAD_TILE; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE; ++j) {
      int row = row_base + i / 4;
      int col = col_base + j / 4;
      if (row < n && col < n) {
        c[row * n + col] = acc[i][j];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
  const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
