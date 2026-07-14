#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int BLOCK_ROWS = 16;
constexpr int BLOCK_COLS = 8;
constexpr int THREAD_TILE_Y = 8;
constexpr int THREAD_TILE_X = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float as[2][TILE_SIZE][TILE_SIZE + 1];
  __shared__ float bs[2][TILE_SIZE][TILE_SIZE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int bx = blockIdx.x;
  const int by = blockIdx.y;

  const int row_base = by * TILE_SIZE + ty * THREAD_TILE_Y;
  const int col_base = bx * TILE_SIZE + tx * THREAD_TILE_X;

  float acc[THREAD_TILE_Y][THREAD_TILE_X];
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_Y; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_X; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;
  const int thread_idx = ty * BLOCK_COLS + tx;
  const int total_threads = BLOCK_ROWS * BLOCK_COLS;
  const int loads_per_thread = (TILE_SIZE * TILE_SIZE + total_threads - 1) / total_threads;

  int write_idx = 0;
  int read_idx = 0;

  // Preload first tile
  #pragma unroll
  for (int l = 0; l < loads_per_thread; ++l) {
    int idx = thread_idx + l * total_threads;
    if (idx < TILE_SIZE * TILE_SIZE) {
      int tile_row = idx / TILE_SIZE;
      int tile_col = idx % TILE_SIZE;
      int global_row = by * TILE_SIZE + tile_row;
      int global_col = tile_col;
      as[write_idx][tile_row][tile_col] = (global_row < n && global_col < n) ? a[global_row * n + global_col] : 0.0f;
      
      global_row = tile_row;
      global_col = bx * TILE_SIZE + tile_col;
      bs[write_idx][tile_row][tile_col] = (global_row < n && global_col < n) ? b[global_row * n + global_col] : 0.0f;
    }
  }
  __syncthreads();

  for (int t = 0; t < num_tiles; ++t) {
    read_idx = write_idx;
    write_idx = 1 - write_idx;

    // Prefetch next tile
    if (t + 1 < num_tiles) {
      #pragma unroll
      for (int l = 0; l < loads_per_thread; ++l) {
        int idx = thread_idx + l * total_threads;
        if (idx < TILE_SIZE * TILE_SIZE) {
          int tile_row = idx / TILE_SIZE;
          int tile_col = idx % TILE_SIZE;
          int global_row = by * TILE_SIZE + tile_row;
          int global_col = (t + 1) * TILE_SIZE + tile_col;
          as[write_idx][tile_row][tile_col] = (global_row < n && global_col < n) ? a[global_row * n + global_col] : 0.0f;
          
          global_row = (t + 1) * TILE_SIZE + tile_row;
          global_col = bx * TILE_SIZE + tile_col;
          bs[write_idx][tile_row][tile_col] = (global_row < n && global_col < n) ? b[global_row * n + global_col] : 0.0f;
        }
      }
    }

    // Compute on current tile
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      float a_reg[THREAD_TILE_Y];
      float b_reg[THREAD_TILE_X];

      #pragma unroll
      for (int i = 0; i < THREAD_TILE_Y; ++i) {
        a_reg[i] = as[read_idx][ty * THREAD_TILE_Y + i][k];
      }

      #pragma unroll
      for (int j = 0; j < THREAD_TILE_X; ++j) {
        b_reg[j] = bs[read_idx][k][tx * THREAD_TILE_X + j];
      }

      #pragma unroll
      for (int i = 0; i < THREAD_TILE_Y; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE_X; ++j) {
          acc[i][j] += a_reg[i] * b_reg[j];
        }
      }
    }

    __syncthreads();
  }

  // Write results
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_Y; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_X; ++j) {
      int row = row_base + i;
      int col = col_base + j;
      if (row < n && col < n) {
        c[row * n + col] = acc[i][j];
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
