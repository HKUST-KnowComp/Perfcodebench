#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  int bx = blockIdx.x;
  int by = blockIdx.y;
  int tx = threadIdx.x;
  int ty = threadIdx.y;

  int row = by * BLOCK_SIZE + ty;
  int col = bx * BLOCK_SIZE + tx;

  __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
  __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];

  float sum = 0.0f;

  for (int tile = 0; tile < gridDim.x; ++tile) {
    // Load tile of A
    int aRow = row;
    int aCol = tile * BLOCK_SIZE + tx;
    if (aRow < n && aCol < n) {
      As[ty][tx] = a[aRow * n + aCol];
    } else {
      As[ty][tx] = 0.0f;
    }

    // Load tile of B
    int bRow = tile * BLOCK_SIZE + ty;
    int bCol = col;
    if (bRow < n && bCol < n) {
      Bs[ty][tx] = b[bRow * n + bCol];
    } else {
      Bs[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Compute partial dot product
    #pragma unroll
    for (int k = 0; k < BLOCK_SIZE; ++k) {
      sum += As[ty][k] * Bs[k][tx];
    }
    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // anonymous namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 block(BLOCK_SIZE, BLOCK_SIZE);
  dim3 grid((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
