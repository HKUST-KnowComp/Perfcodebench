#include "interface.h"

#include <cuda_runtime.h>

namespace {

// 32x32 tiled SGEMM kernel using shared memory.
// Each thread computes one output element. This is a strong baseline for
// square FP32 GEMM and substantially faster than the naive implementation.
constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  float sum = 0.0f;

  const int num_tiles = (n + TILE - 1) / TILE;
  for (int t = 0; t < num_tiles; ++t) {
    const int a_col = t * TILE + tx;
    const int b_row = t * TILE + ty;

    As[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
    Bs[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(As[ty][k], Bs[k][tx], sum);
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE, TILE);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
