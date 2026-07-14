#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_M = 32;
constexpr int TILE_N = 32;
constexpr int TILE_K = 8;
constexpr int THREADS_X = 16;
constexpr int THREADS_Y = 16;
constexpr int WPT_M = 2;
constexpr int WPT_N = 2;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float As[TILE_M][TILE_K];
  __shared__ float Bs[TILE_K][TILE_N];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int block_row = blockIdx.y * TILE_M;
  const int block_col = blockIdx.x * TILE_N;

  const int row0 = block_row + ty * WPT_M;
  const int col0 = block_col + tx * WPT_N;

  float acc[WPT_M][WPT_N];
#pragma unroll
  for (int i = 0; i < WPT_M; ++i) {
#pragma unroll
    for (int j = 0; j < WPT_N; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  for (int kb = 0; kb < n; kb += TILE_K) {
    const int linear_tid = ty * THREADS_X + tx;

#pragma unroll
    for (int load_idx = 0; load_idx < 2; ++load_idx) {
      const int idx = linear_tid + load_idx * (THREADS_X * THREADS_Y);
      const int a_row = idx / TILE_K;
      const int a_col = idx - a_row * TILE_K;
      const int g_row = block_row + a_row;
      const int g_col = kb + a_col;
      As[a_row][a_col] = (g_row < n && g_col < n) ? a[g_row * n + g_col] : 0.0f;
    }

#pragma unroll
    for (int load_idx = 0; load_idx < 2; ++load_idx) {
      const int idx = linear_tid + load_idx * (THREADS_X * THREADS_Y);
      const int b_row = idx / TILE_N;
      const int b_col = idx - b_row * TILE_N;
      const int g_row = kb + b_row;
      const int g_col = block_col + b_col;
      Bs[b_row][b_col] = (g_row < n && g_col < n) ? b[g_row * n + g_col] : 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE_K; ++k) {
      float a_frag[WPT_M];
      float b_frag[WPT_N];

#pragma unroll
      for (int i = 0; i < WPT_M; ++i) {
        a_frag[i] = As[ty * WPT_M + i][k];
      }
#pragma unroll
      for (int j = 0; j < WPT_N; ++j) {
        b_frag[j] = Bs[k][tx * WPT_N + j];
      }

#pragma unroll
      for (int i = 0; i < WPT_M; ++i) {
#pragma unroll
        for (int j = 0; j < WPT_N; ++j) {
          acc[i][j] += a_frag[i] * b_frag[j];
        }
      }
    }

    __syncthreads();
  }

#pragma unroll
  for (int i = 0; i < WPT_M; ++i) {
    const int row = row0 + i;
    if (row < n) {
#pragma unroll
      for (int j = 0; j < WPT_N; ++j) {
        const int col = col0 + j;
        if (col < n) {
          c[row * n + col] = acc[i][j];
        }
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  const dim3 block(THREADS_X, THREADS_Y);
  const dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_M - 1) / TILE_M);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
