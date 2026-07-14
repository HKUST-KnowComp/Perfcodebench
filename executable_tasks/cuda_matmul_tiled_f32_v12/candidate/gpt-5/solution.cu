#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef TILE_SIZE
#define TILE_SIZE 32
#endif

// Read-only cached load helper (falls back to normal load on older arch)
static __device__ __forceinline__ float ldgf(const float* ptr) {
#if __CUDA_ARCH__ >= 350
  return __ldg(ptr);
#else
  return *ptr;
#endif
}

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * blockDim.x + tx;
  const int row = blockIdx.y * blockDim.y + ty;

  __shared__ float As[TILE_SIZE][TILE_SIZE];
  __shared__ float Bs[TILE_SIZE][TILE_SIZE];

  float sum = 0.0f;

  const int tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

#pragma unroll
  for (int t = 0; t < tiles; ++t) {
    const int a_col = t * TILE_SIZE + tx;
    const int b_row = t * TILE_SIZE + ty;

    // Load A tile element if in bounds, else 0
    if (row < n && a_col < n) {
      As[ty][tx] = ldgf(a + row * n + a_col);
    } else {
      As[ty][tx] = 0.0f;
    }

    // Load B tile element if in bounds, else 0
    if (b_row < n && col < n) {
      Bs[ty][tx] = ldgf(b + b_row * n + col);
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += As[ty][k] * Bs[k][tx];
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE_SIZE, TILE_SIZE);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
