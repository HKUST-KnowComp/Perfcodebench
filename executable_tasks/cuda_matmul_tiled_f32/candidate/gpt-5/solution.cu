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
  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  // Padding +1 in the minor dimension to reduce shared memory bank conflicts
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  float sum = 0.0f;

  for (int t = 0; t < n; t += TILE) {
    int a_col = t + tx;
    int b_row = t + ty;

    // Load tiles with bounds checks; threads outside matrix load zeros
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

inline dim3 make_grid_dim(int n, int tile) {
  return dim3((n + tile - 1) / tile, (n + tile - 1) / tile, 1);
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  // Choose tile size based on problem size to balance occupancy and per-block work
  if (n >= 256) {
    const int TILE = 32;
    dim3 block(TILE, TILE);
    dim3 grid = make_grid_dim(n, TILE);
    for (int i = 0; i < iters; ++i) {
      matmul_tiled_kernel<TILE><<<grid, block>>>(a, b, c, n);
    }
  } else {
    const int TILE = 16;
    dim3 block(TILE, TILE);
    dim3 grid = make_grid_dim(n, TILE);
    for (int i = 0; i < iters; ++i) {
      matmul_tiled_kernel<TILE><<<grid, block>>>(a, b, c, n);
    }
  }
}
