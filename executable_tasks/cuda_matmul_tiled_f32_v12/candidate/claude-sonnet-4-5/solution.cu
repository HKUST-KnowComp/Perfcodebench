#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int THREAD_TILE_M = 8;
constexpr int THREAD_TILE_N = 8;
constexpr int BLOCK_M = TILE_SIZE;
constexpr int BLOCK_N = TILE_SIZE;
constexpr int BLOCK_K = TILE_SIZE;
constexpr int THREADS_PER_BLOCK = (TILE_SIZE / THREAD_TILE_M) * (TILE_SIZE / THREAD_TILE_N);

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float as[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float bs[TILE_SIZE][TILE_SIZE + 1];

  const int tid = threadIdx.x;
  const int warp_id = tid / 32;
  const int lane_id = tid % 32;
  
  const int thread_row = tid / (TILE_SIZE / THREAD_TILE_N);
  const int thread_col = tid % (TILE_SIZE / THREAD_TILE_N);
  
  const int block_row = blockIdx.y;
  const int block_col = blockIdx.x;
  
  const int c_row_start = block_row * BLOCK_M + thread_row * THREAD_TILE_M;
  const int c_col_start = block_col * BLOCK_N + thread_col * THREAD_TILE_N;
  
  float acc[THREAD_TILE_M][THREAD_TILE_N];
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      acc[i][j] = 0.0f;
    }
  }
  
  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;
  
  for (int tile_idx = 0; tile_idx < num_tiles; ++tile_idx) {
    const int tile_k_start = tile_idx * TILE_SIZE;
    
    #pragma unroll
    for (int load_iter = 0; load_iter < (TILE_SIZE * TILE_SIZE) / THREADS_PER_BLOCK; ++load_iter) {
      const int load_idx = tid + load_iter * THREADS_PER_BLOCK;
      const int load_row = load_idx / TILE_SIZE;
      const int load_col = load_idx % TILE_SIZE;
      
      const int a_row = block_row * BLOCK_M + load_row;
      const int a_col = tile_k_start + load_col;
      const int b_row = tile_k_start + load_row;
      const int b_col = block_col * BLOCK_N + load_col;
      
      as[load_row][load_col] = (a_row < n && a_col < n) ? a[a_row * n + a_col] : 0.0f;
      bs[load_row][load_col] = (b_row < n && b_col < n) ? b[b_row * n + b_col] : 0.0f;
    }
    
    __syncthreads();
    
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      float a_reg[THREAD_TILE_M];
      float b_reg[THREAD_TILE_N];
      
      #pragma unroll
      for (int i = 0; i < THREAD_TILE_M; ++i) {
        a_reg[i] = as[thread_row * THREAD_TILE_M + i][k];
      }
      
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        b_reg[j] = bs[k][thread_col * THREAD_TILE_N + j];
      }
      
      #pragma unroll
      for (int i = 0; i < THREAD_TILE_M; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE_N; ++j) {
          acc[i][j] += a_reg[i] * b_reg[j];
        }
      }
    }
    
    __syncthreads();
  }
  
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      const int c_row = c_row_start + i;
      const int c_col = c_col_start + j;
      if (c_row < n && c_col < n) {
        c[c_row * n + c_col] = acc[i][j];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(THREADS_PER_BLOCK);
  const dim3 grid((n + BLOCK_N - 1) / BLOCK_N, (n + BLOCK_M - 1) / BLOCK_M);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
