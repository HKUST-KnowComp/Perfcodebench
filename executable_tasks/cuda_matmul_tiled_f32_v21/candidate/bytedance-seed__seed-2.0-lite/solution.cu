#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  __shared__ float s_a[BLOCK_SIZE][BLOCK_SIZE];
  __shared__ float s_b[BLOCK_SIZE][BLOCK_SIZE];
  
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * BLOCK_SIZE + ty;
  const int col = blockIdx.x * BLOCK_SIZE + tx;
  
  float sum = 0.0f;
  const int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
  
  for (int k_tile = 0; k_tile < num_tiles; ++k_tile) {
    const int a_k = k_tile * BLOCK_SIZE + tx;
    if (row < n && a_k < n) {
      s_a[ty][tx] = a[row * n + a_k];
    } else {
      s_a[ty][tx] = 0.0f;
    }
    
    const int b_k = k_tile * BLOCK_SIZE + ty;
    if (b_k < n && col < n) {
      s_b[ty][tx] = b[b_k * n + col];
    } else {
      s_b[ty][tx] = 0.0f;
    }
    
    __syncthreads();
    
    for (int k = 0; k < BLOCK_SIZE; ++k) {
      sum += s_a[ty][k] * s_b[k][tx];
    }
    
    __syncthreads();
  }
  
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}