#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef TILE_SIZE
#define TILE_SIZE 32
#endif

// Tiled shared-memory matrix multiplication kernel
// Computes C = A x B for square matrices of size n x n
__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int TILE = TILE_SIZE;
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  // Pad second dimension by +1 to reduce shared memory bank conflicts
  __shared__ float As[TILE_SIZE][TILE_SIZE + 1];
  __shared__ float Bs[TILE_SIZE][TILE_SIZE + 1];

  float sum = 0.0f;

  // Loop over tiles of the K dimension
  for (int t = 0; t < n; t += TILE) {
    // Load A tile (row, t + tx)
    float a_val = 0.0f;
    if (row < n) {
      int kA = t + tx;
      if (kA < n) {
        a_val = a[row * n + kA];
      }
    }
    As[ty][tx] = a_val;

    // Load B tile (t + ty, col)
    float b_val = 0.0f;
    if (col < n) {
      int kB = t + ty;
      if (kB < n) {
        b_val = b[kB * n + col];
      }
    }
    Bs[ty][tx] = b_val;

    __syncthreads();

    // Compute partial products for this tile
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
  const int TILE = TILE_SIZE;
  dim3 block(TILE, TILE);
  dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
