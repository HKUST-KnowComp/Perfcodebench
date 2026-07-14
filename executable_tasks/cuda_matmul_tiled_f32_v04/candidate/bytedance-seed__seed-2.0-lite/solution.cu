#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  __shared__ float a_tile[BLOCK_SIZE][BLOCK_SIZE];
  __shared__ float b_tile[BLOCK_SIZE][BLOCK_SIZE];

  const int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
  const int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;
  float sum = 0.0f;

  const int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
  for (int tile_k = 0; tile_k < num_tiles; ++tile_k) {
    // Load A tile fragment to shared memory
    const int a_global_row = row;
    const int a_global_col = tile_k * BLOCK_SIZE + threadIdx.x;
    a_tile[threadIdx.y][threadIdx.x] = (a_global_row < n && a_global_col < n)
      ? a[a_global_row * n + a_global_col] : 0.0f;

    // Load B tile fragment to shared memory
    const int b_global_row = tile_k * BLOCK_SIZE + threadIdx.y;
    const int b_global_col = col;
    b_tile[threadIdx.y][threadIdx.x] = (b_global_row < n && b_global_col < n)
      ? b[b_global_row * n + b_global_col] : 0.0f;

    __syncthreads();

    // Accumulate partial sum from shared memory
    for (int k = 0; k < BLOCK_SIZE; ++k) {
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
  const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}