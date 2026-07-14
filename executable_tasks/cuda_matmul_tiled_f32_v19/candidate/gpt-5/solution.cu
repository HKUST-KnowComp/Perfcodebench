#include "interface.h"

#include <cuda_runtime.h>

namespace {

#ifndef LDG
#if defined(__CUDA_ARCH__) && (__CUDA_ARCH__ >= 350)
#define LDG(ptr) __ldg(ptr)
#else
#define LDG(ptr) (*(ptr))
#endif
#endif

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int row = blockIdx.y * TILE + ty;
  const int col = blockIdx.x * TILE + tx;

  // Add +1 padding on the second dimension to avoid shared memory bank conflicts
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  float sum = 0.0f;

  for (int t = 0; t < n; t += TILE) {
    // Load tile of A
    float aElem = 0.0f;
    int aCol = t + tx;
    if (row < n && aCol < n) {
      aElem = LDG(&a[row * n + aCol]);
    }
    As[ty][tx] = aElem;

    // Load tile of B
    float bElem = 0.0f;
    int bRow = t + ty;
    if (bRow < n && col < n) {
      bElem = LDG(&b[bRow * n + col]);
    }
    Bs[ty][tx] = bElem;

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(As[ty][k], Bs[k][tx], sum);
    }

    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 block(TILE, TILE);
  dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
