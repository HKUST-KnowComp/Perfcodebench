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
      const int load_row = ty + i * BLOCK_SIZE;
      #pragma unroll
      for (int j = 0; j < THREAD_TILE / 2; ++j) {
        const int load_col = tx + j * BLOCK_SIZE;
        const int a_row = by * TILE_SIZE + load_row;
        const int a_col = tile_offset + load_col;
        const int b_row = tile_offset + load_row;
        const int b_col = bx * TILE_SIZE + load_col;
        
        as[load_row][load_col] = (a_row < n && a_col < n) ? a[a_row * n + a_col] : 0.0f;
        bs[load_row][load_col] = (b_row < n && b_col < n) ? b[b_row * n + b_col] : 0.0f;
      }
    }
    
    __syncthreads();
    
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      #pragma unroll
      for (int i = 0; i < THREAD_TILE; ++i) {
        const int a_row = ty + i * (TILE_SIZE / THREAD_TILE);
        float a_val = as[a_row][k];
        #pragma unroll
        for (int j = 0; j < THREAD_TILE; ++j) {
          const int b_col = tx + j * (TILE_SIZE / THREAD_TILE);
          acc[i][j] += a_val * bs[k][b_col];
        }
      }
    }
    
    __syncthreads();
  }
  
  #pragma unroll
  for (int i = 0; i < THREAD_TILE; ++i) {
    const int out_row = row_base + i * (TILE_SIZE / THREAD_TILE);
    #pragma unroll
    for (int j = 0; j < THREAD_TILE; ++j) {
      const int out_col = col_base + j * (TILE_SIZE / THREAD_TILE);
      if (out_row < n && out_col < n) {
        c[out_row * n + out_col] = acc[i][j];
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
