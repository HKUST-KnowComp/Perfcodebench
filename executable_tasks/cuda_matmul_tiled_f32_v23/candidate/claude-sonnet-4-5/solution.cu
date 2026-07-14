#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int BLOCK_SIZE = 8;
constexpr int REG_TILE = 4;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float as[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float bs[TILE_SIZE][TILE_SIZE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int bx = blockIdx.x;
  const int by = blockIdx.y;

  const int row_base = by * TILE_SIZE + ty * REG_TILE;
  const int col_base = bx * TILE_SIZE + tx * REG_TILE;

  float acc[REG_TILE][REG_TILE];
  #pragma unroll
  for (int i = 0; i < REG_TILE; ++i) {
    #pragma unroll
    for (int j = 0; j < REG_TILE; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int t = 0; t < num_tiles; ++t) {
    const int tile_offset = t * TILE_SIZE;

    #pragma unroll
    for (int i = 0; i < REG_TILE; ++i) {
      #pragma unroll
      for (int j = 0; j < REG_TILE; ++j) {
        const int row = by * TILE_SIZE + ty * REG_TILE + i;
        const int col = tile_offset + tx * REG_TILE + j;
        as[ty * REG_TILE + i][tx * REG_TILE + j] = (row < n && col < n) ? a[row * n + col] : 0.0f;

        const int row_b = tile_offset + ty * REG_TILE + i;
        const int col_b = bx * TILE_SIZE + tx * REG_TILE + j;
        bs[ty * REG_TILE + i][tx * REG_TILE + j] = (row_b < n && col_b < n) ? b[row_b * n + col_b] : 0.0f;
      }
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      float a_reg[REG_TILE];
      float b_reg[REG_TILE];

      #pragma unroll
      for (int i = 0; i < REG_TILE; ++i) {
        a_reg[i] = as[ty * REG_TILE + i][k];
      }

      #pragma unroll
      for (int j = 0; j < REG_TILE; ++j) {
        b_reg[j] = bs[k][tx * REG_TILE + j];
      }

      #pragma unroll
      for (int i = 0; i < REG_TILE; ++i) {
        #pragma unroll
        for (int j = 0; j < REG_TILE; ++j) {
          acc[i][j] += a_reg[i] * b_reg[j];
        }
      }
    }

    __syncthreads();
  }

  #pragma unroll
  for (int i = 0; i < REG_TILE; ++i) {
    #pragma unroll
    for (int j = 0; j < REG_TILE; ++j) {
      const int row = row_base + i;
      const int col = col_base + j;
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
