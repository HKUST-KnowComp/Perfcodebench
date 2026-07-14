#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int tile_size = 16;
  __shared__ float a_tile[tile_size][tile_size];
  __shared__ float b_tile[tile_size][tile_size];
  const int col = blockIdx.x * tile_size + threadIdx.x;
  const int row = blockIdx.y * tile_size + threadIdx.y;
  float sum = 0.0f;
  for (int t = 0; t < (n + tile_size - 1) / tile_size; ++t) {
    if (row < n && t * tile_size + threadIdx.x < n) {
      a_tile[threadIdx.y][threadIdx.x] = a[row * n + t * tile_size + threadIdx.x];
    } else {
      a_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }
    if (t * tile_size + threadIdx.y < n && col < n) {
      b_tile[threadIdx.y][threadIdx.x] = b[(t * tile_size + threadIdx.y) * n + col];
    } else {
      b_tile[threadIdx.y][threadIdx.x] = 0.0f;
    }
    __syncthreads();
    for (int k = 0; k < tile_size; ++k) {
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
  const dim3 block(16, 16);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}