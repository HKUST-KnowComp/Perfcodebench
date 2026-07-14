#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;
constexpr int THREAD_TILE_M = 8;
constexpr int THREAD_TILE_N = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float as[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float bs[TILE_SIZE][TILE_SIZE + 1];

  const int bx = blockIdx.x;
  const int by = blockIdx.y;
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int threads_per_block = TILE_SIZE * TILE_SIZE / (THREAD_TILE_M * THREAD_TILE_N);
  const int tid = ty * (TILE_SIZE / THREAD_TILE_M) + tx;

  const int row_base = by * TILE_SIZE + (tid / (TILE_SIZE / THREAD_TILE_N)) * THREAD_TILE_M;
  const int col_base = bx * TILE_SIZE + (tid % (TILE_SIZE / THREAD_TILE_N)) * THREAD_TILE_N;

  float acc[THREAD_TILE_M][THREAD_TILE_N];
  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int t = 0; t < num_tiles; ++t) {
    const int tile_offset = t * TILE_SIZE;

    #pragma unroll
    for (int i = 0; i < THREAD_TILE_M; ++i) {
      #pragma unroll
      for (int j = 0; j < THREAD_TILE_N; ++j) {
        const int load_tid = (i * THREAD_TILE_N + j) * threads_per_block + tid;
        const int load_row = load_tid / TILE_SIZE;
        const int load_col = load_tid % TILE_SIZE;
        
        if (load_row < TILE_SIZE && load_col < TILE_SIZE) {
          const int global_row = by * TILE_SIZE + load_row;
          const int global_col = tile_offset + load_col;
          as[load_row][load_col] = (global_row < n && global_col < n) ? a[global_row * n + global_col] : 0.0f;
          
          const int global_row_b = tile_offset + load_row;
          const int global_col_b = bx * TILE_SIZE + load_col;
          bs[load_row][load_col] = (global_row_b < n && global_col_b < n) ? b[global_row_b * n + global_col_b] : 0.0f;
        }
      }
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      #pragma unroll
      for (int i = 0; i < THREAD_TILE_M; ++i) {
        const float a_val = as[row_base - by * TILE_SIZE + i][k];
        #pragma unroll
        for (int j = 0; j < THREAD_TILE_N; ++j) {
          acc[i][j] += a_val * bs[k][col_base - bx * TILE_SIZE + j];
        }
      }
    }

    __syncthreads();
  }

  #pragma unroll
  for (int i = 0; i < THREAD_TILE_M; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE_N; ++j) {
      const int global_row = row_base + i;
      const int global_col = col_base + j;
      if (global_row < n && global_col < n) {
        c[global_row * n + global_col] = acc[i][j];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE_SIZE / THREAD_TILE_N, TILE_SIZE / THREAD_TILE_M);
  const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
