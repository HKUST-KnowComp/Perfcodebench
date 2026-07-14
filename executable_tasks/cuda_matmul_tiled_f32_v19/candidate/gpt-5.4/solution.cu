#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_M = 64;
constexpr int TILE_N = 64;
constexpr int TILE_K = 16;
constexpr int THREADS_X = 16;
constexpr int THREADS_Y = 16;
constexpr int WPT_M = 4;
constexpr int WPT_N = 4;

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

  const int row_base = block_row + ty * WPT_M;
  const int col_base = block_col + tx * WPT_N;

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
    for (int idx = linear_tid; idx < TILE_M * TILE_K; idx += THREADS_X * THREADS_Y) {
      const int r = idx / TILE_K;
      const int k = idx - r * TILE_K;
      const int gr = block_row + r;
      const int gk = kb + k;
      As[r][k] = (gr < n && gk < n) ? a[gr * n + gk] : 0.0f;
    }

#pragma unroll
    for (int idx = linear_tid; idx < TILE_K * TILE_N; idx += THREADS_X * THREADS_Y) {
      const int k = idx / TILE_N;
      const int ccol = idx - k * TILE_N;
      const int gk = kb + k;
      const int gc = block_col + ccol;
      Bs[k][ccol] = (gk < n && gc < n) ? b[gk * n + gc] : 0.0f;
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
    const int row = row_base + i;
    if (row < n) {
#pragma unroll
      for (int j = 0; j < WPT_N; ++j) {
        const int col = col_base + j;
        if (col < n) {
          c[row * n + col] = acc[i][j];
        }
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(THREADS_X, THREADS_Y);
  const dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_M - 1) / TILE_M);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
