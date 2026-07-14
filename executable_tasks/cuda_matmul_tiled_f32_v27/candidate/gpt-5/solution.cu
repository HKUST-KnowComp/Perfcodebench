#include "interface.h"

#include <cuda_runtime.h>

namespace {

template<int TILE>
__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * TILE + tx;
  const int row = blockIdx.y * TILE + ty;

  __shared__ float As[TILE][TILE];
  __shared__ float Bs[TILE][TILE];

  float sum = 0.0f;

  const int row_base = row * n; // valid only if row < n

  for (int t = 0; t < n; t += TILE) {
    // Load a tile of A and B into shared memory with bounds checks
    float aval = 0.0f;
    float bval = 0.0f;

    int a_col = t + tx;
    if (row < n && a_col < n) {
      aval = a[row_base + a_col];
    }

    int b_row = t + ty;
    if (b_row < n && col < n) {
      bval = b[b_row * n + col];
    }

    As[ty][tx] = aval;
    Bs[ty][tx] = bval;

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

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  // Choose tile size based on problem size; 32 is generally faster for larger n
  if (n >= 32) {
    const int TILE = 32;
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
      matmul_tiled_kernel<TILE><<<grid, block>>>(a, b, c, n);
    }
  } else {
    const int TILE = 16;
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
      matmul_tiled_kernel<TILE><<<grid, block>>>(a, b, c, n);
    }
  }
}
