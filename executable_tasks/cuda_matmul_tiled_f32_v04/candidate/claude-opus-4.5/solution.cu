#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 64;
constexpr int BLOCK = 16;
constexpr int THREAD_TILE = 4;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float As[TILE][TILE];
  __shared__ float Bs[TILE][TILE];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;

  const int row0 = blockIdx.y * TILE + ty * THREAD_TILE;
  const int col0 = blockIdx.x * TILE + tx * THREAD_TILE;

  float acc[THREAD_TILE][THREAD_TILE];
  #pragma unroll
  for (int i = 0; i < THREAD_TILE; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  const int numTiles = (n + TILE - 1) / TILE;

  for (int t = 0; t < numTiles; ++t) {
    const int tileStart = t * TILE;

    // Load A tile: each thread loads THREAD_TILE rows x THREAD_TILE cols
    #pragma unroll
    for (int i = 0; i < THREAD_TILE; ++i) {
      #pragma unroll
      for (int j = 0; j < THREAD_TILE; ++j) {
        int r = ty * THREAD_TILE + i;
        int c_idx = tx * THREAD_TILE + j;
        int globalRow = blockIdx.y * TILE + r;
        int globalCol = tileStart + c_idx;
        if (globalRow < n && globalCol < n) {
          As[r][c_idx] = a[globalRow * n + globalCol];
        } else {
          As[r][c_idx] = 0.0f;
        }
      }
    }

    // Load B tile
    #pragma unroll
    for (int i = 0; i < THREAD_TILE; ++i) {
      #pragma unroll
      for (int j = 0; j < THREAD_TILE; ++j) {
        int r = ty * THREAD_TILE + i;
        int c_idx = tx * THREAD_TILE + j;
        int globalRow = tileStart + r;
        int globalCol = blockIdx.x * TILE + c_idx;
        if (globalRow < n && globalCol < n) {
          Bs[r][c_idx] = b[globalRow * n + globalCol];
        } else {
          Bs[r][c_idx] = 0.0f;
        }
      }
    }

    __syncthreads();

    // Compute partial products
    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      float aReg[THREAD_TILE];
      float bReg[THREAD_TILE];
      #pragma unroll
      for (int i = 0; i < THREAD_TILE; ++i) {
        aReg[i] = As[ty * THREAD_TILE + i][k];
      }
      #pragma unroll
      for (int j = 0; j < THREAD_TILE; ++j) {
        bReg[j] = Bs[k][tx * THREAD_TILE + j];
      }
      #pragma unroll
      for (int i = 0; i < THREAD_TILE; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE; ++j) {
          acc[i][j] += aReg[i] * bReg[j];
        }
      }
    }

    __syncthreads();
  }

  // Write results
  #pragma unroll
  for (int i = 0; i < THREAD_TILE; ++i) {
    #pragma unroll
    for (int j = 0; j < THREAD_TILE; ++j) {
      int globalRow = row0 + i;
      int globalCol = col0 + j;
      if (globalRow < n && globalCol < n) {
        c[globalRow * n + globalCol] = acc[i][j];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(BLOCK, BLOCK);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
