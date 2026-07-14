#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef TILE_DIM
#define TILE_DIM 32
#endif

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE_DIM + ty;
  const int col = blockIdx.x * TILE_DIM + tx;

  __shared__ float As[TILE_DIM][TILE_DIM + 1];
  __shared__ float Bs[TILE_DIM][TILE_DIM + 1];

  float acc = 0.0f;

  // Loop over tiles of A and B
  for (int t = 0; t < n; t += TILE_DIM) {
    // Load tile from A: A[row, t + tx]
    if (row < n && (t + tx) < n) {
#if __CUDA_ARCH__ >= 350
      As[ty][tx] = __ldg(&A[row * n + (t + tx)]);
#else
      As[ty][tx] = A[row * n + (t + tx)];
#endif
    } else {
      As[ty][tx] = 0.0f;
    }

    // Load tile from B: B[t + ty, col]
    if (col < n && (t + ty) < n) {
#if __CUDA_ARCH__ >= 350
      Bs[ty][tx] = __ldg(&B[(t + ty) * n + col]);
#else
      Bs[ty][tx] = B[(t + ty) * n + col];
#endif
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE_DIM; ++k) {
      acc = fmaf(As[ty][k], Bs[k][tx], acc);
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    C[row * n + col] = acc;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  if (n <= 0 || iters <= 0) return;
  const dim3 block(TILE_DIM, TILE_DIM, 1);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y, 1);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
