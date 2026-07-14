#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int THREAD_TILE_M = 8;
constexpr int THREAD_TILE_N = 8;
constexpr int BLOCK_M = TILE_SIZE;
constexpr int BLOCK_N = TILE_SIZE;
constexpr int BLOCK_K = TILE_SIZE;
constexpr int THREADS_PER_BLOCK_X = BLOCK_N / THREAD_TILE_N;
constexpr int THREADS_PER_BLOCK_Y = BLOCK_M / THREAD_TILE_M;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, 
                                     const float* __restrict__ b, 
                                     float* __restrict__ c, 
                                     int n) {
  __shared__ float as[BLOCK_M][BLOCK_K];
  __shared__ float bs[BLOCK_K][BLOCK_N];
  
  const int tid_x = threadIdx.x;
  const int tid_y = threadIdx.y;
  const int block_row = blockIdx.y;
  const int block_col = blockIdx.x;
  
  const int row_base = block_row * BLOCK_M + tid_y * THREAD_TILE_M;
  const int col_base = block_col * BLOCK_N + tid_x * THREAD_TILE_N;
  
  float acc[THREAD_TILE_M][THREAD_TILE_N];
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      acc[i][j] = 0.0f;
    }
  }
  
  const int num_tiles = (n + BLOCK_K - 1) / BLOCK_K;
  
  for (int tile = 0; tile < num_tiles; ++tile) {
    const int k_base = tile * BLOCK_K;
    
    #pragma unroll
    for (int i = 0; i < THREAD_TILE_M; ++i) {
      const int row = block_row * BLOCK_M + tid_y * THREAD_TILE_M + i;
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        const int col = k_base + tid_x * THREAD_TILE_N + j;
        if (row < n && col < n) {
          as[tid_y * THREAD_TILE_M + i][tid_x * THREAD_TILE_N + j] = a[row * n + col];
        } else {
          as[tid_y * THREAD_TILE_M + i][tid_x * THREAD_TILE_N + j] = 0.0f;
        }
      }
    }
    
    #pragma unroll
    for (int i = 0; i < THREAD_TILE_M; ++i) {
      const int row = k_base + tid_y * THREAD_TILE_M + i;
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        const int col = block_col * BLOCK_N + tid_x * THREAD_TILE_N + j;
        if (row < n && col < n) {
          bs[tid_y * THREAD_TILE_M + i][tid_x * THREAD_TILE_N + j] = b[row * n + col];
        } else {
          bs[tid_y * THREAD_TILE_M + i][tid_x * THREAD_TILE_N + j] = 0.0f;
        }
      }
    }
    
    __syncthreads();
    
    #pragma unroll
    for (int k = 0; k < BLOCK_K; ++k) {
      #pragma unroll
      for (int i = 0; i < THREAD_TILE_M; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE_N; ++j) {
          acc[i][j] += as[tid_y * THREAD_TILE_M + i][k] * bs[k][tid_x * THREAD_TILE_N + j];
        }
      }
    }
    
    __syncthreads();
  }
  
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    const int row = row_base + i;
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      const int col = col_base + j;
      if (row < n && col < n) {
        c[row * n + col] = acc[i][j];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(THREADS_PER_BLOCK_X, THREADS_PER_BLOCK_Y);
  const dim3 grid((n + BLOCK_N - 1) / BLOCK_N, (n + BLOCK_M - 1) / BLOCK_M);
  
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
