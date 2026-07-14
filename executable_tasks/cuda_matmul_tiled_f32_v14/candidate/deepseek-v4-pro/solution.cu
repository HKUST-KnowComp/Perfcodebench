#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tiled matrix multiplication kernel (C = A * B)
// Each block computes a TILE_DIM x TILE_DIM sub-matrix of C.
// Shared memory is used to cache tiles of A and B.
__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                   const float* __restrict__ b,
                                   float* __restrict__ c,
                                   int n) {
  constexpr int TILE_DIM = 32;

  // Block row and column indices
  const int col = blockIdx.x * TILE_DIM + threadIdx.x;
  const int row = blockIdx.y * TILE_DIM + threadIdx.y;

  // Shared memory for tiles of A and B
  __shared__ float As[TILE_DIM][TILE_DIM];
  __shared__ float Bs[TILE_DIM][TILE_DIM];

  // Accumulator for this thread
  float sum = 0.0f;

  // Loop over tiles of the K dimension
  for (int kt = 0; kt < n; kt += TILE_DIM) {
    // Cooperatively load tile of A into shared memory
    if (row < n && (kt + threadIdx.x) < n) {
      As[threadIdx.y][threadIdx.x] = a[row * n + (kt + threadIdx.x)];
    } else {
      As[threadIdx.y][threadIdx.x] = 0.0f;
    }

    // Cooperatively load tile of B into shared memory
    if ((kt + threadIdx.y) < n && col < n) {
      Bs[threadIdx.y][threadIdx.x] = b[(kt + threadIdx.y) * n + col];
    } else {
      Bs[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Compute partial dot product using the current tiles
    #pragma unroll
    for (int k = 0; k < TILE_DIM; ++k) {
      sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
    }

    __syncthreads();
  }

  // Write the result to global memory
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  constexpr int TILE_DIM = 32;
  const dim3 block(TILE_DIM, TILE_DIM);
  const dim3 grid((n + TILE_DIM - 1) / TILE_DIM,
                  (n + TILE_DIM - 1) / TILE_DIM);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}