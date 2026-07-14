#include "interface.h"

#include <cuda_runtime.h>

namespace {

template<int TILE>
__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float sA[TILE][TILE];
  __shared__ float sB[TILE][TILE];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * TILE + tx;
  const int row = blockIdx.y * TILE + ty;

  const int rowBase = row * n;

  float sum = 0.0f;

  for (int t = 0; t < n; t += TILE) {
    float a_val = 0.0f;
    float b_val = 0.0f;

    // Load A tile element if in bounds
    if (row < n) {
      int a_col = t + tx;
      if (a_col < n) {
        a_val = a[rowBase + a_col];
      }
    }

    // Load B tile element if in bounds
    if (col < n) {
      int b_row = t + ty;
      if (b_row < n) {
        b_val = b[b_row * n + col];
      }
    }

    sA[ty][tx] = a_val;
    sB[ty][tx] = b_val;

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(sA[ty][k], sB[k][tx], sum);
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[rowBase + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  // Choose tile size based on matrix size to improve occupancy on small matrices
  const int tile = (n >= 32) ? 32 : 16;

  dim3 block(tile, tile);
  dim3 grid((n + tile - 1) / tile, (n + tile - 1) / tile);

  // Prefer shared memory to help tiled matmul
  if (tile == 32) {
    cudaFuncSetCacheConfig(matmul_tiled_kernel<32>, cudaFuncCachePreferShared);
  } else {
    cudaFuncSetCacheConfig(matmul_tiled_kernel<16>, cudaFuncCachePreferShared);
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (tile == 32) {
      matmul_tiled_kernel<32><<<grid, block>>>(a, b, c, n);
    } else {
      matmul_tiled_kernel<16><<<grid, block>>>(a, b, c, n);
    }
  }
}
