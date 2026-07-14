#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int THREAD_TILE_M = 8;
constexpr int THREAD_TILE_N = 8;
constexpr int BLOCK_ROWS = 16;
constexpr int BLOCK_COLS = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float as[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float bs[TILE_SIZE][TILE_SIZE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int bx = blockIdx.x;
  const int by = blockIdx.y;

  const int row_base = by * TILE_SIZE + ty * THREAD_TILE_M;
  const int col_base = bx * TILE_SIZE + tx * THREAD_TILE_N;

  float acc[THREAD_TILE_M][THREAD_TILE_N];
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int tile = 0; tile < num_tiles; ++tile) {
    const int tile_offset = tile * TILE_SIZE;

    #pragma unroll
    for (int i = 0; i < THREAD_TILE_M; ++i) {
      const int row = ty * THREAD_TILE_M + i;
      const int global_row = by * TILE_SIZE + row;
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        const int col = tx * THREAD_TILE_N + j;
        const int global_col = tile_offset + col;
        if (global_row < n && global_col < n) {
          as[row][col] = a[global_row * n + global_col];
        } else {
          as[row][col] = 0.0f;
        }
      }
    }

    #pragma unroll
    for (int i = 0; i < THREAD_TILE_M; ++i) {
      const int row = ty * THREAD_TILE_M + i;
      const int global_row = tile_offset + row;
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        const int col = tx * THREAD_TILE_N + j;
        const int global_col = bx * TILE_SIZE + col;
        if (global_row < n && global_col < n) {
          bs[row][col] = b[global_row * n + global_col];
        } else {
          bs[row][col] = 0.0f;
        }
      }
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      #pragma unroll
      for (int i = 0; i < THREAD_TILE_M; ++i) {
        const float a_val = as[ty * THREAD_TILE_M + i][k];
        #pragma unroll
        for (int j = 0; j < THREAD_TILE_N; ++j) {
          acc[i][j] += a_val * bs[k][tx * THREAD_TILE_N + j];
        }
      }
    }

    __syncthreads();
  }

  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    const int global_row = row_base + i;
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      const int global_col = col_base + j;
      if (global_row < n && global_col < n) {
        c[global_row * n + global_col] = acc[i][j];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(BLOCK_COLS, BLOCK_ROWS);
  const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
