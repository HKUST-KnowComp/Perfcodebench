#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int TILE_SIZE = 16;
  __shared__ float a_shared[TILE_SIZE][TILE_SIZE];
  __shared__ float b_shared[TILE_SIZE][TILE_SIZE];

  const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  const int col = blockIdx.x * TILE_SIZE + threadIdx.x;

  if (row >= n || col >= n) {
    return;
  }

  float sum = 0.0f;

  for (int k = 0; k < n; k += TILE_SIZE) {
    // Load A tile
    if ((k + threadIdx.x) < n) {
      a_shared[threadIdx.y][threadIdx.x] = a[row * n + (k + threadIdx.x)];
    } else {
      a_shared[threadIdx.y][threadIdx.x] = 0.0f;
    }

    // Load B tile
    if ((k + threadIdx.y) < n) {
      b_shared[threadIdx.y][threadIdx.x] = b[(k + threadIdx.y) * n + col];
    } else {
      b_shared[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Compute partial sum
    for (int i = 0; i < TILE_SIZE; ++i) {
      sum += a_shared[threadIdx.y][i] * b_shared[i][threadIdx.x];
    }

    __syncthreads();
  }

  c[row * n + col] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(16, 16);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}