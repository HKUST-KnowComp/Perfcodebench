#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef TILE_DIM
#define TILE_DIM 32
#endif

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE_DIM + ty;
  const int col = blockIdx.x * TILE_DIM + tx;

  // Pad second dimension by +1 to reduce shared memory bank conflicts
  __shared__ float As[TILE_DIM][TILE_DIM + 1];
  __shared__ float Bs[TILE_DIM][TILE_DIM + 1];

  float sum = 0.0f;

  for (int t = 0; t < n; t += TILE_DIM) {
    // Load A tile
    if (row < n) {
      int a_col = t + tx;
      As[ty][tx] = (a_col < n) ? a[row * n + a_col] : 0.0f;
    } else {
      As[ty][tx] = 0.0f;
    }

    // Load B tile
    if (col < n) {
      int b_row = t + ty;
      Bs[ty][tx] = (b_row < n) ? b[b_row * n + col] : 0.0f;
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE_DIM; ++k) {
      sum += As[ty][k] * Bs[k][tx];
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  if (n <= 0 || iters <= 0) return;
  const dim3 block(TILE_DIM, TILE_DIM);
  const dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
