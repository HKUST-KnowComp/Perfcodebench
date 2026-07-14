#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
  __shared__ float tile_a[BLOCK_SIZE][BLOCK_SIZE + 1];
  __shared__ float tile_b[BLOCK_SIZE][BLOCK_SIZE + 1];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int row = blockIdx.y * BLOCK_SIZE + ty;
  const int col = blockIdx.x * BLOCK_SIZE + tx;

  float sum = 0.0f;

  for (int k_tile = 0; k_tile < (n + BLOCK_SIZE - 1) / BLOCK_SIZE; ++k_tile) {
    const int a_k = k_tile * BLOCK_SIZE + tx;
    if (row < n && a_k < n) {
      tile_a[ty][tx] = a[row * n + a_k];
    } else {
      tile_a[ty][tx] = 0.0f;
    }

    const int b_k = k_tile * BLOCK_SIZE + ty;
    if (b_k < n && col < n) {
      tile_b[ty][tx] = b[b_k * n + col];
    } else {
      tile_b[ty][tx] = 0.0f;
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < BLOCK_SIZE; ++k) {
      sum += tile_a[ty][k] * tile_b[k][tx];
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