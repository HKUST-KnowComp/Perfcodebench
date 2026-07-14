#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;                 // tile width & height
constexpr int BLOCK = 128;               // threads per block

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
  // 2-D tile indices
  const int tileRow = blockIdx.y;
  const int tileCol = blockIdx.x;

  // 1-D thread index inside 128-thread block
  const int tid = threadIdx.x;

  // row & col inside the tile
  const int row = tid / TILE;
  const int col = tid % TILE;

  // global starting row & col for this tile
  const int globalRow = tileRow * TILE + row;
  const int globalCol = tileCol * TILE + col;

  // shared memory for tiles of A and B
  __shared__ float tileA[TILE][TILE];
  __shared__ float tileB[TILE][TILE];

  float acc = 0.0f;

  // loop over tiles along the K dimension
  for (int t = 0; t < (n + TILE - 1) / TILE; ++t) {
    // load A tile: A[globalRow][t*TILE + col]
    int aRow = globalRow;
    int aCol = t * TILE + col;
    tileA[row][col] = (aRow < n && aCol < n) ? A[aRow * n + aCol] : 0.0f;

    // load B tile: B[t*TILE + row][globalCol]
    int bRow = t * TILE + row;
    int bCol = globalCol;
    tileB[row][col] = (bRow < n && bCol < n) ? B[bRow * n + bCol] : 0.0f;

    __syncthreads();

    // compute partial dot product inside tile
    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      acc += tileA[row][k] * tileB[k][col];
    }
    __syncthreads();
  }

  // write back
  if (globalRow < n && globalCol < n) {
    C[globalRow * n + globalCol] = acc;
  }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 block(BLOCK);
  dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int i = 0; i < iters; ++i) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}