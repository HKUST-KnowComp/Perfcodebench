#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  __shared__ float As[TILE][TILE + 1];  // +1 to reduce bank conflicts
  __shared__ float Bs[TILE][TILE + 1];

  float sum = 0.0f;

  const int tiles = (n + TILE - 1) / TILE;

  for (int t = 0; t < tiles; ++t) {
    const int aCol = t * TILE + tx;
    const int bRow = t * TILE + ty;

    float aVal = 0.0f;
    float bVal = 0.0f;

    if (row < n && aCol < n) {
      aVal = A[row * n + aCol];
    }
    if (bRow < n && col < n) {
      bVal = B[bRow * n + col];
    }

    As[ty][tx] = aVal;
    Bs[ty][tx] = bVal;

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(As[ty][k], Bs[k][tx], sum);
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    C[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE, TILE);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

  // Hint the runtime to prefer shared memory for this kernel
  cudaFuncSetCacheConfig(matmul_tiled_kernel, cudaFuncCachePreferShared);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
