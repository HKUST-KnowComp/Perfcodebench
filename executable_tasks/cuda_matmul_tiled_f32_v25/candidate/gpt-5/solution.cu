#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled32_kernel(const float* __restrict__ A,
                                      const float* __restrict__ B,
                                      float* __restrict__ C,
                                      int N) {
  // Use padding to reduce shared memory bank conflicts
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  float acc = 0.0f;

  // Loop over tiles of K dimension
  for (int t = 0; t < N; t += TILE) {
    float aVal = 0.0f;
    float bVal = 0.0f;

    if (row < N) {
      int aCol = t + tx;
      if (aCol < N) {
        aVal = A[row * N + aCol];
      }
    }

    if (col < N) {
      int bRow = t + ty;
      if (bRow < N) {
        bVal = B[bRow * N + col];
      }
    }

    As[ty][tx] = aVal;
    Bs[ty][tx] = bVal;

    __syncthreads();

    // Compute using the loaded tiles
#pragma unroll
    for (int k = 0; k < TILE; ++k) {
      acc = fmaf(As[ty][k], Bs[k][tx], acc);
    }

    __syncthreads();
  }

  if (row < N && col < N) {
    C[row * N + col] = acc;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  if (n <= 0 || iters <= 0) return;
  const dim3 block(TILE, TILE);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled32_kernel<<<grid, block>>>(a, b, c, n);
  }
}
