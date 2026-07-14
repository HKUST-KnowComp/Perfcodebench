#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_M = 32;
constexpr int TILE_N = 32;
constexpr int TILE_K = 8;
constexpr int THREADS_X = 16;
constexpr int THREADS_Y = 16;

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

    #pragma unroll
    for (int i = 0; i < 2; ++i) {
      const int idx = linear_tid + i * THREADS_X * THREADS_Y;
      const int a_r = idx / TILE_K;
      const int a_c = idx - a_r * TILE_K;
      const int g_r = block_row + a_r;
      const int g_c = kb + a_c;
      As[a_r][a_c] = (g_r < n && g_c < n) ? a[g_r * n + g_c] : 0.0f;
    }

    #pragma unroll
    for (int i = 0; i < 1; ++i) {
      const int idx = linear_tid + i * THREADS_X * THREADS_Y;
      const int b_r = idx / TILE_N;
      const int b_c = idx - b_r * TILE_N;
      const int g_r = kb + b_r;
      const int g_c = block_col + b_c;
      Bs[b_r][b_c] = (g_r < n && g_c < n) ? b[g_r * n + g_c] : 0.0f;
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_K; ++k) {
      const float a00 = As[ty][k];
      const float a10 = As[ty + THREADS_Y][k];
      const float b00 = Bs[k][tx];
      const float b01 = Bs[k][tx + THREADS_X];

      acc00 += a00 * b00;
      acc01 += a00 * b01;
      acc10 += a10 * b00;
      acc11 += a10 * b01;
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
