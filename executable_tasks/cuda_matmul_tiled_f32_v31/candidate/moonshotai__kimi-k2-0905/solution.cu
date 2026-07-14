#include "interface.h"
#include <cuda_runtime.h>

namespace {

// 32×32 tile, each thread computes 8 elements → 128 threads/block
constexpr int TILE = 32;
constexpr int ELEMS_PER_THREAD = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
  // Shared memory for 32×32 tiles of A and B
  __shared__ float tileA[TILE][TILE + 1]; // +1 to avoid bank conflicts
  __shared__ float tileB[TILE][TILE + 1];

  // Each thread handles ELEMS_PER_THREAD consecutive columns in its row
  const int tileRow = threadIdx.y;
  const int tileCol = threadIdx.x * ELEMS_PER_THREAD;
  const int globalRow = blockIdx.y * TILE + tileRow;

  float accum[ELEMS_PER_THREAD] = {0.0f};

  const int numTiles = (n + TILE - 1) / TILE;
  for (int t = 0; t < numTiles; ++t) {
    // Load tileA: transpose while loading to get coalesced reads
    #pragma unroll
    for (int e = 0; e < ELEMS_PER_THREAD; ++e) {
      int col = tileCol + e;
      int gRow = globalRow;
      int gCol = t * TILE + tileRow;
      tileA[col][tileRow] = (gRow < n && gCol < n) ? A[gRow * n + gCol] : 0.0f;
    }

    // Load tileB: coalesced read along rows
    #pragma unroll
    for (int e = 0; e < ELEMS_PER_THREAD; ++e) {
      int col = tileCol + e;
      int gRow = t * TILE + tileRow;
      int gCol = blockIdx.x * TILE + col;
      tileB[tileRow][col] = (gRow < n && gCol < n) ? B[gRow * n + gCol] : 0.0f;
    }
    __syncthreads();

    // Compute partial dot products
    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      #pragma unroll
      for (int e = 0; e < ELEMS_PER_THREAD; ++e) {
        accum[e] += tileA[tileCol + e][k] * tileB[k][tileCol + e];
      }
    }
    __syncthreads();
  }

  // Write back results
  #pragma unroll
  for (int e = 0; e < ELEMS_PER_THREAD; ++e) {
    int col = tileCol + e;
    int gRow = globalRow;
    int gCol = blockIdx.x * TILE + col;
    if (gRow < n && gCol < n) {
      C[gRow * n + gCol] = accum[e];
    }
  }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 block(TILE / ELEMS_PER_THREAD, TILE);
  dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int i = 0; i < iters; ++i) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}