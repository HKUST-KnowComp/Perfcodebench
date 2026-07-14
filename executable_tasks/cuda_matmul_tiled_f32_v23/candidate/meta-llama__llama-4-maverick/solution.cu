#include "interface.h"

#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 16

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  __shared__ float a_tile[TILE_SIZE][TILE_SIZE];
  __shared__ float b_tile[TILE_SIZE][TILE_SIZE];
  const int col = blockIdx.x * TILE_SIZE + threadIdx.x;
  const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  float sum = 0.0f;
  for (int k = 0; k < n; k += TILE_SIZE) {
    if (row < n && k + threadIdx.x < n) {
      a_tile[threadIdx.y][threadIdx.x] = a[row * n + k + threadIdx.x];
    } else {
      a_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }
    if (k + threadIdx.y < n && col < n) {
      b_tile[threadIdx.y][threadIdx.x] = b[(k + threadIdx.y) * n + col];
    } else {
      b_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }
    __syncthreads();
    for (int i = 0; i < TILE_SIZE; ++i) {
      sum += a_tile[threadIdx.y][i] * b_tile[i][threadIdx.x];
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