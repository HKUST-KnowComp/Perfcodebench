#include "interface.h"

#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 16

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  __shared__ float As[TILE_SIZE][TILE_SIZE];
  __shared__ float Bs[TILE_SIZE][TILE_SIZE];

  int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  int col = blockIdx.x * TILE_SIZE + threadIdx.x;

  float sum = 0.0f;

  for (int k = 0; k < n; k += TILE_SIZE) {
    if (row < n && col < n) {
      As[threadIdx.y][threadIdx.x] = a[row * n + k + threadIdx.x];
      Bs[threadIdx.y][threadIdx.x] = b[(k + threadIdx.y) * n + col];
    }
    __syncthreads();

    for (int i = 0; i < TILE_SIZE; ++i) {
      sum += As[threadIdx.y][i] * Bs[i][threadIdx.x];
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