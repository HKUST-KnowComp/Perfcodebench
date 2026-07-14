#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int THREAD_TILE_M = 8;
constexpr int THREAD_TILE_N = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, 
                                     const float* __restrict__ b, 
                                     float* __restrict__ c, 
                                     int n) {
  __shared__ float as[TILE_SIZE][TILE_SIZE];
  __shared__ float bs[TILE_SIZE][TILE_SIZE];
  
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int bx = blockIdx.x;
  const int by = blockIdx.y;
  
  const int threads_per_tile_m = TILE_SIZE / THREAD_TILE_M;
  const int threads_per_tile_n = TILE_SIZE / THREAD_TILE_N;
  const int thread_id = ty * blockDim.x + tx;
  const int num_threads = blockDim.x * blockDim.y;
  
  const int row_base = by * TILE_SIZE + (thread_id / threads_per_tile_n) * THREAD_TILE_M;
  const int col_base = bx * TILE_SIZE + (thread_id % threads_per_tile_n) * THREAD_TILE_N;
  
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
    
    for (int load_id = thread_id; load_id < TILE_SIZE * TILE_SIZE; load_id += num_threads) {
      const int load_row = load_id / TILE_SIZE;
      const int load_col = load_id % TILE_SIZE;
      const int a_row = by * TILE_SIZE + load_row;
      const int a_col = tile_offset + load_col;
      const int b_row = tile_offset + load_row;
      const int b_col = bx * TILE_SIZE + load_col;
      
      as[load_row][load_col] = (a_row < n && a_col < n) ? a[a_row * n + a_col] : 0.0f;
      bs[load_row][load_col] = (b_row < n && b_col < n) ? b[b_row * n + b_col] : 0.0f;
    }
    
    __syncthreads();
    
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      float a_reg[THREAD_TILE_M];
      float b_reg[THREAD_TILE_N];
      
      const int local_row = (thread_id / threads_per_tile_n) * THREAD_TILE_M;
      const int local_col = (thread_id % threads_per_tile_n) * THREAD_TILE_N;
      
      #pragma unroll
      for (int i = 0; i < THREAD_TILE_M; ++i) {
        a_reg[i] = as[local_row + i][k];
      }
      
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        b_reg[j] = bs[k][local_col + j];
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
  const int threads_per_tile_m = TILE_SIZE / THREAD_TILE_M;
  const int threads_per_tile_n = TILE_SIZE / THREAD_TILE_N;
  const dim3 block(threads_per_tile_n, threads_per_tile_m);
  const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
  
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
