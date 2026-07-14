#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int TILE_SIZE = 16;
  __shared__ float shared_a[TILE_SIZE][TILE_SIZE];
  __shared__ float shared_b[TILE_SIZE][TILE_SIZE];

  const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  const int col = blockIdx.x * TILE_SIZE + threadIdx.x;

  if (row >= n || col >= n) {
    return;
  }

  float sum = 0.0f;

  for (int k = 0; k < n; k += TILE_SIZE) {
    int a_col = k + threadIdx.x;
    if (a_col < n) {
      shared_a[threadIdx.y][threadIdx.x] = a[row * n + a_col];
    } else {
      shared_a[threadIdx.y][threadIdx.x] = 0.0f;
    }

    int b_row = k + threadIdx.y;
    if (b_row < n) {
      shared_b[threadIdx.y][threadIdx.x] = b[b_row * n + col];
    } else {
      shared_b[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    for (int i = 0; i < TILE_SIZE; ++i) {
      sum += shared_a[threadIdx.y][i] * shared_b[i][threadIdx.x];
    }
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