#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int row = blockIdx.y * 16 + threadIdx.y;
  const int col = blockIdx.x * 16 + threadIdx.x;

  if (row >= n || col >= n) {
    return;
  }

  __shared__ float As[16][16];
  __shared__ float Bs[16][16];

  float sum = 0.0f;

  for (int k = 0; k < n; k += 16) {
    // Load A's tile into shared memory
    if (row + threadIdx.y < n && k + threadIdx.x < n) {
      As[threadIdx.y][threadIdx.x] = a[(row + threadIdx.y) * n + (k + threadIdx.x)];
    } else {
      As[threadIdx.y][threadIdx.x] = 0.0f;
    }

    // Load B's tile into shared memory
    if (k + threadIdx.y < n && col + threadIdx.x < n) {
      Bs[threadIdx.y][threadIdx.x] = b[(k + threadIdx.y) * n + (col + threadIdx.x)];
    } else {
      Bs[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Compute partial sum for this tile
    for (int i = 0; i < 16; ++i) {
      sum += As[threadIdx.y][i] * Bs[i][threadIdx.x];
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