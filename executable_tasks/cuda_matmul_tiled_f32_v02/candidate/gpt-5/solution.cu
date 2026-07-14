#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int row = blockIdx.y * TILE + threadIdx.y;
  const int col = blockIdx.x * TILE + threadIdx.x;

  __shared__ float As[TILE][TILE];
  __shared__ float Bs[TILE][TILE];

  float sum = 0.0f;

  for (int t = 0; t < n; t += TILE) {
    float aval = 0.0f;
    float bval = 0.0f;

    const int a_col = t + threadIdx.x;
    const int b_row = t + threadIdx.y;

    if (row < n && a_col < n) {
      aval = a[row * n + a_col];
    }
    if (b_row < n && col < n) {
      bval = b[b_row * n + col];
    }

    As[threadIdx.y][threadIdx.x] = aval;
    Bs[threadIdx.y][threadIdx.x] = bval;

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(As[threadIdx.y][k], Bs[k][threadIdx.x], sum);
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
