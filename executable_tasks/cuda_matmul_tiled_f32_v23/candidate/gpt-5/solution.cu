#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef TILE_SIZE
#define TILE_SIZE 32
#endif

// Tiled FP32 matrix multiplication: C = A x B
// Square matrices of size n x n located in device memory
// Each block computes a TILE_SIZE x TILE_SIZE tile of C using shared memory
__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * TILE_SIZE + tx;
  const int row = blockIdx.y * TILE_SIZE + ty;

  // Pad second dimension to reduce shared memory bank conflicts
  __shared__ float As[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float Bs[TILE_SIZE][TILE_SIZE + 1];

  float sum = 0.0f;

  // Loop over tiles
  for (int m = 0; m < n; m += TILE_SIZE) {
    // Load A tile: A[row, m + tx]
    if (row < n && (m + tx) < n) {
      As[ty][tx] = a[row * n + (m + tx)];
    } else {
      As[ty][tx] = 0.0f;
    }

    // Load B tile: B[m + ty, col]
    if ((m + ty) < n && col < n) {
      Bs[ty][tx] = b[(m + ty) * n + col];
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Compute partial dot product for the tile
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
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
  const dim3 block(TILE_SIZE, TILE_SIZE);
  const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
