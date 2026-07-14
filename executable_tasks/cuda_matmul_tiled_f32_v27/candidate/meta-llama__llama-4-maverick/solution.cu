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
  for (int tile = 0; tile < (n + TILE_SIZE - 1) / TILE_SIZE; ++tile) {
    if (row < n && tile * TILE_SIZE + threadIdx.x < n) {
      a_tile[threadIdx.y][threadIdx.x] = a[row * n + tile * TILE_SIZE + threadIdx.x];
    } else {
      a_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }
    if (tile * TILE_SIZE + threadIdx.y < n && col < n) {
      b_tile[threadIdx.y][threadIdx.x] = b[(tile * TILE_SIZE + threadIdx.y) * n + col];
    } else {
      b_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }
    __syncthreads();
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += a_tile[threadIdx.y][k] * b_tile[k][threadIdx.x];
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
  cudaDeviceSynchronize();
}