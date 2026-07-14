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
  
  const int row_base = by * TILE_SIZE + ty;
  const int col_base = bx * TILE_SIZE + tx;
  
  float acc[THREAD_TILE][THREAD_TILE] = {0.0f};
  
  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;
  
  for (int tile = 0; tile < num_tiles; ++tile) {
    const int tile_offset = tile * TILE_SIZE;
    
    #pragma unroll
    for (int i = 0; i < THREAD_TILE / 2; ++i) {
      const int row = ty + i * BLOCK_SIZE;
      const int col = tx;
      const int global_row = by * TILE_SIZE + row;
      const int global_col = tile_offset + col;
      
      if (global_row < n && global_col < n) {
        as[row][col] = a[global_row * n + global_col];
      } else {
        as[row][col] = 0.0f;
      }
      
      const int row2 = ty + i * BLOCK_SIZE;
      const int col2 = tx;
      const int global_row2 = tile_offset + row2;
      const int global_col2 = bx * TILE_SIZE + col2;
      
      if (global_row2 < n && global_col2 < n) {
        bs[row2][col2] = b[global_row2 * n + global_col2];
      } else {
        bs[row2][col2] = 0.0f;
      }
    }
    
    __syncthreads();
    
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      #pragma unroll
      for (int i = 0; i < THREAD_TILE / 2; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE / 2; ++j) {
          const int row_idx = ty + i * BLOCK_SIZE;
          const int col_idx = tx + j * BLOCK_SIZE;
          acc[i][j] += as[row_idx][k] * bs[k][col_idx];
        }
      }
    }
    
    __syncthreads();
  }
  
  #pragma unroll
  for (int i = 0; i < THREAD_TILE / 2; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE / 2; ++j) {
      const int global_row = row_base + i * BLOCK_SIZE;
      const int global_col = col_base + j * BLOCK_SIZE;
      if (global_row < n && global_col < n) {
        c[global_row * n + global_col] = acc[i][j];
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
