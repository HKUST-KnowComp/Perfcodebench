#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Tile size; 32x32 uses 1024 threads per block and generally offers good perf on many GPUs.
constexpr int TILE = 32;

// Tiled shared-memory matrix multiplication kernel
__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int N) {
  // Compute global row/col this thread is responsible for
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  // Shared memory tiles with padding (+1) to reduce bank conflicts
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  float sum = 0.0f;

  const int numTiles = (N + TILE - 1) / TILE;
  const int rowN = row * N; // base index for A/C row when in-bounds

  for (int t = 0; t < numTiles; ++t) {
    const int aCol = t * TILE + tx;
    const int bRow = t * TILE + ty;

    // Load A tile element if in bounds; else 0
    if (row < N && aCol < N) {
      As[ty][tx] = A[rowN + aCol];
    } else {
      As[ty][tx] = 0.0f;
    }

    // Load B tile element if in bounds; else 0
    if (bRow < N && col < N) {
      Bs[ty][tx] = B[bRow * N + col];
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Compute partial dot product for this tile
    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(As[ty][k], Bs[k][tx], sum);
    }

    __syncthreads();
  }

  if (row < N && col < N) {
    C[rowN + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  // Prefer shared memory for this kernel to improve performance
  cudaFuncSetCacheConfig(matmul_tiled_kernel, cudaFuncCachePreferShared);

  const dim3 block(TILE, TILE);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
