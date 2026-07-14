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
  __shared__ float As[TILE_M][TILE_K + 1];
  __shared__ float Bs[TILE_K][TILE_N + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int block_row = blockIdx.y * TILE_M;
  const int block_col = blockIdx.x * TILE_N;

  const int row0 = block_row + ty;
  const int row1 = block_row + ty + THREADS_Y;
  const int col0 = block_col + tx;
  const int col1 = block_col + tx + THREADS_X;

  float acc00 = 0.0f;
  float acc01 = 0.0f;
  float acc10 = 0.0f;
  float acc11 = 0.0f;

  for (int kb = 0; kb < n; kb += TILE_K) {
    const int linear_tid = ty * THREADS_X + tx;
    const int a_loads_per_tile = (TILE_M * TILE_K) / (THREADS_X * THREADS_Y);
    const int b_loads_per_tile = (TILE_K * TILE_N) / (THREADS_X * THREADS_Y);

#pragma unroll
    for (int i = 0; i < a_loads_per_tile; ++i) {
      const int idx = linear_tid + i * THREADS_X * THREADS_Y;
      const int r = idx / TILE_K;
      const int k = idx - r * TILE_K;
      const int gr = block_row + r;
      const int gk = kb + k;
      As[r][k] = (gr < n && gk < n) ? a[gr * n + gk] : 0.0f;
    }

#pragma unroll
    for (int i = 0; i < b_loads_per_tile; ++i) {
      const int idx = linear_tid + i * THREADS_X * THREADS_Y;
      const int k = idx / TILE_N;
      const int ccol = idx - k * TILE_N;
      const int gk = kb + k;
      const int gc = block_col + ccol;
      Bs[k][ccol] = (gk < n && gc < n) ? b[gk * n + gc] : 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE_K; ++k) {
      const float a0 = As[ty][k];
      const float a1 = As[ty + THREADS_Y][k];
      const float b0 = Bs[k][tx];
      const float b1 = Bs[k][tx + THREADS_X];

      acc00 += a0 * b0;
      acc01 += a0 * b1;
      acc10 += a1 * b0;
      acc11 += a1 * b1;
    }

    __syncthreads();
  }

  if (row0 < n && col0 < n) c[row0 * n + col0] = acc00;
  if (row0 < n && col1 < n) c[row0 * n + col1] = acc01;
  if (row1 < n && col0 < n) c[row1 * n + col0] = acc10;
  if (row1 < n && col1 < n) c[row1 * n + col1] = acc11;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(THREADS_X, THREADS_Y);
  const dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_M - 1) / TILE_M);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
