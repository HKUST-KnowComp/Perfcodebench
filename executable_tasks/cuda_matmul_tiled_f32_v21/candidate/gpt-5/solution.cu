#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int N) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * TILE + tx;
  const int row = blockIdx.y * TILE + ty;

  // Shared memory tiles with padding to mitigate bank conflicts
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  float sum = 0.0f;

  if (row < N || col < N) {
    const int rowN = row * N; // valid only if row < N, but we guard accesses
    const int numTiles = (N + TILE - 1) / TILE;

    for (int t = 0; t < numTiles; ++t) {
      const int aCol = t * TILE + tx;
      const int bRow = t * TILE + ty;

      // Load A tile
      if (row < N && aCol < N) {
        As[ty][tx] = A[rowN + aCol];
      } else {
        As[ty][tx] = 0.0f;
      }

      // Load B tile
      if (bRow < N && col < N) {
        Bs[ty][tx] = B[bRow * N + col];
      } else {
        Bs[ty][tx] = 0.0f;
      }

      __syncthreads();

      #pragma unroll
      for (int k = 0; k < TILE; ++k) {
        sum += As[ty][k] * Bs[k][tx];
      }

      __syncthreads();
    }

    if (row < N && col < N) {
      C[rowN + col] = sum;
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE, TILE);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

  // Prefer shared memory for this kernel to improve shared mem bandwidth
  cudaFuncSetCacheConfig(matmul_tiled_kernel, cudaFuncCachePreferShared);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
