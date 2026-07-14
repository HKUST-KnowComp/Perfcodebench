#include "interface.h"
#include <cuda_runtime.h>

namespace {

// 32x32 tile size chosen for good occupancy and shared-memory capacity
constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  // Shared memory for tiles of A and B
  __shared__ float tileA[TILE][TILE + 1]; // +1 to avoid bank conflicts
  __shared__ float tileB[TILE][TILE + 1];

  int tx = threadIdx.x; // column in tile
  int ty = threadIdx.y; // row in tile

  int row = blockIdx.y * TILE + ty; // global row
  int col = blockIdx.x * TILE + tx; // global col

  float sum = 0.0f;

  // Loop over tiles
  for (int t = 0; t < (n + TILE - 1) / TILE; ++t) {
    // Load tile of A: rows in [blockIdx.y*TILE, blockIdx.y*TILE+TILE-1]
    int aRow = blockIdx.y * TILE + ty;
    int aCol = t * TILE + tx;
    tileA[ty][tx] = (aRow < n && aCol < n) ? a[aRow * n + aCol] : 0.0f;

    // Load tile of B: cols in [blockIdx.x*TILE, blockIdx.x*TILE+TILE-1]
    int bRow = t * TILE + ty;
    int bCol = blockIdx.x * TILE + tx;
    tileB[ty][tx] = (bRow < n && bCol < n) ? b[bRow * n + bCol] : 0.0f;

    __syncthreads();

    // Compute partial dot-product for this tile
    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum += tileA[ty][k] * tileB[k][tx];
    }
    __syncthreads();
  }

  // Write back tile of C
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 blockDim(TILE, TILE);
  dim3 gridDim((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int i = 0; i < iters; ++i) {
    matmul_tiled_kernel<<<gridDim, blockDim>>>(a, b, c, n);
  }
}