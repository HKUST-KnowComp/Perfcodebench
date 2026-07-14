#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  const int TILE_SIZE = 32;
  __shared__ float As[TILE_SIZE][TILE_SIZE];
  __shared__ float Bs[TILE_SIZE][TILE_SIZE];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int col = blockIdx.x * TILE_SIZE + tx;
  const int row = blockIdx.y * TILE_SIZE + ty;

  float sum = 0.0f;
  const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int t = 0; t < num_tiles; ++t) {
    // Load A tile to shared memory with bounds protection
    if (row < n && t * TILE_SIZE + tx < n) {
      As[ty][tx] = a[row * n + t * TILE_SIZE + tx];
    } else {
      As[ty][tx] = 0.0f;
    }
    // Load B tile to shared memory with bounds protection
    if (t * TILE_SIZE + ty < n && col < n) {
      Bs[ty][tx] = b[(t * TILE_SIZE + ty) * n + col];
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Accumulate partial sum from current tile
    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += As[ty][k] * Bs[k][tx];
    }

    __syncthreads();
  }

  // Write final result to global memory
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const int TILE_SIZE = 32;
  const dim3 block(TILE_SIZE, TILE_SIZE);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}