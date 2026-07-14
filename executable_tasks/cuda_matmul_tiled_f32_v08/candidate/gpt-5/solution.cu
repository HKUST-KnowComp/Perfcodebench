#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef TILE_DIM
#define TILE_DIM 32
#endif

// Fast path kernel: assumes n is a multiple of TILE_DIM, so no bound checks needed.
__global__ void matmul_tiled_aligned(const float* __restrict__ A,
                                     const float* __restrict__ B,
                                     float* __restrict__ C,
                                     int n) {
  const int row = blockIdx.y * TILE_DIM + threadIdx.y;
  const int col = blockIdx.x * TILE_DIM + threadIdx.x;

  __shared__ float As[TILE_DIM][TILE_DIM + 1];
  __shared__ float Bs[TILE_DIM][TILE_DIM + 1];

  float sum = 0.0f;

  // Loop over tiles of K dimension
  for (int t = 0; t < n; t += TILE_DIM) {
    // Coalesced loads into shared memory
    As[threadIdx.y][threadIdx.x] = A[row * n + (t + threadIdx.x)];
    Bs[threadIdx.y][threadIdx.x] = B[(t + threadIdx.y) * n + col];

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_DIM; ++k) {
      sum = fmaf(As[threadIdx.y][k], Bs[k][threadIdx.x], sum);
    }

    __syncthreads();
  }

  C[row * n + col] = sum;
}

// Generic kernel with bounds checking for edge tiles when n is not multiple of TILE_DIM.
__global__ void matmul_tiled_generic(const float* __restrict__ A,
                                     const float* __restrict__ B,
                                     float* __restrict__ C,
                                     int n) {
  const int row = blockIdx.y * TILE_DIM + threadIdx.y;
  const int col = blockIdx.x * TILE_DIM + threadIdx.x;

  __shared__ float As[TILE_DIM][TILE_DIM + 1];
  __shared__ float Bs[TILE_DIM][TILE_DIM + 1];

  float sum = 0.0f;

  for (int t = 0; t < n; t += TILE_DIM) {
    int aCol = t + threadIdx.x;
    int bRow = t + threadIdx.y;

    if (row < n && aCol < n) {
      As[threadIdx.y][threadIdx.x] = A[row * n + aCol];
    } else {
      As[threadIdx.y][threadIdx.x] = 0.0f;
    }

    if (bRow < n && col < n) {
      Bs[threadIdx.y][threadIdx.x] = B[bRow * n + col];
    } else {
      Bs[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE_DIM; ++k) {
      sum = fmaf(As[threadIdx.y][k], Bs[k][threadIdx.x], sum);
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    C[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE_DIM, TILE_DIM);
  const bool aligned = (n % TILE_DIM) == 0;
  const dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);

  for (int iter = 0; iter < iters; ++iter) {
    if (aligned) {
      const dim3 grid_aligned(n / TILE_DIM, n / TILE_DIM);
      matmul_tiled_aligned<<<grid_aligned, block>>>(a, b, c, n);
    } else {
      matmul_tiled_generic<<<grid, block>>>(a, b, c, n);
    }
  }
}
