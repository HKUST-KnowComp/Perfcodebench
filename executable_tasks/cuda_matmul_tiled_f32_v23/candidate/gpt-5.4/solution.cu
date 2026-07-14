#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_M = 32;
constexpr int TILE_N = 32;
constexpr int TILE_K = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float As[TILE_M][TILE_K + 1];
  __shared__ float Bs[TILE_K][TILE_N + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int row = blockIdx.y * TILE_M + ty;
  const int col = blockIdx.x * TILE_N + tx;

  float acc = 0.0f;

  for (int k0 = 0; k0 < n; k0 += TILE_K) {
    if (row < n && (k0 + tx) < n && tx < TILE_K) {
      As[ty][tx] = a[row * n + (k0 + tx)];
    }
    if (ty < TILE_K && col < n && (k0 + ty) < n) {
      Bs[ty][tx] = b[(k0 + ty) * n + col];
    }

    if (tx >= TILE_K) {
      // no-op for A load lanes
    }
    if (ty >= TILE_K) {
      // no-op for B load lanes
    }

    if (!(row < n && (k0 + tx) < n && tx < TILE_K) && tx < TILE_K) {
      As[ty][tx] = 0.0f;
    }
    if (!(ty < TILE_K && col < n && (k0 + ty) < n) && ty < TILE_K) {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE_K; ++k) {
      acc += As[ty][k] * Bs[k][tx];
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = acc;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE_N, TILE_M);
  const dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_M - 1) / TILE_M);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
