#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_M = 32;
constexpr int TILE_N = 32;
constexpr int TILE_K = 32;
constexpr int THREADS_X = 8;
constexpr int THREADS_Y = 32;
constexpr int COLS_PER_THREAD = 4;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int block_row = blockIdx.y * TILE_M;
  const int block_col = blockIdx.x * TILE_N;

  const int row = block_row + ty;
  const int col_base = block_col + tx * COLS_PER_THREAD;

  __shared__ float As[TILE_M][TILE_K];
  __shared__ float Bs[TILE_K][TILE_N];

  float acc[COLS_PER_THREAD] = {0.f, 0.f, 0.f, 0.f};

  // Cooperative loading mapping:
  // 256 threads load 1024 elements for A tile and 1024 elements for B tile.
  // Each thread loads 4 elements from A and 4 elements from B.
  const int linear_tid = ty * THREADS_X + tx;

  for (int k0 = 0; k0 < n; k0 += TILE_K) {
    #pragma unroll
    for (int i = 0; i < 4; ++i) {
      const int idx = linear_tid + i * (THREADS_X * THREADS_Y);

      const int a_r = idx / TILE_K;
      const int a_c = idx - a_r * TILE_K;
      const int g_a_r = block_row + a_r;
      const int g_a_c = k0 + a_c;
      As[a_r][a_c] = (g_a_r < n && g_a_c < n) ? a[g_a_r * n + g_a_c] : 0.0f;

      const int b_r = idx / TILE_N;
      const int b_c = idx - b_r * TILE_N;
      const int g_b_r = k0 + b_r;
      const int g_b_c = block_col + b_c;
      Bs[b_r][b_c] = (g_b_r < n && g_b_c < n) ? b[g_b_r * n + g_b_c] : 0.0f;
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_K; ++k) {
      const float a_val = As[ty][k];
      #pragma unroll
      for (int j = 0; j < COLS_PER_THREAD; ++j) {
        acc[j] = fmaf(a_val, Bs[k][tx * COLS_PER_THREAD + j], acc[j]);
      }
    }

    __syncthreads();
  }

  if (row < n) {
    #pragma unroll
    for (int j = 0; j < COLS_PER_THREAD; ++j) {
      const int col = col_base + j;
      if (col < n) {
        c[row * n + col] = acc[j];
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
