#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float tileA[TILE_SIZE][TILE_SIZE];
  __shared__ float tileB[TILE_SIZE][TILE_SIZE];

  int tx = threadIdx.x;
  int ty = threadIdx.y;
  int col = blockIdx.x * TILE_SIZE + tx;
  int row = blockIdx.y * TILE_SIZE + ty;

  float sum = 0.0f;
  int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

  for (int t = 0; t < num_tiles; ++t) {
    int tile_start = t * TILE_SIZE;

    // Load A tile
    if (row < n && tile_start + tx < n) {
      tileA[ty][tx] = a[row * n + (tile_start + tx)];
    } else {
      tileA[ty][tx] = 0.0f;
    }

    // Load B tile
    if (tile_start + ty < n && col < n) {
      tileB[ty][tx] = b[(tile_start + ty) * n + col];
    } else {
      tileB[ty][tx] = 0.0f;
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_SIZE; ++k) {
      sum += tileA[ty][k] * tileB[k][tx];
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // anonymous namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 block(TILE_SIZE, TILE_SIZE);
  dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
