#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int N) {
  const int row = blockIdx.y * TILE + threadIdx.y;
  const int col = blockIdx.x * TILE + threadIdx.x;

  // Pad second dimension by +1 to minimize shared memory bank conflicts
  __shared__ float As[TILE][TILE + 1];
  __shared__ float Bs[TILE][TILE + 1];

  float sum = 0.0f;

  for (int t = 0; t < N; t += TILE) {
    float a_val = 0.0f;
    float b_val = 0.0f;

    int a_col = t + threadIdx.x;
    int b_row = t + threadIdx.y;

    if (row < N && a_col < N) {
      a_val = A[row * N + a_col];
    }
    if (b_row < N && col < N) {
      b_val = B[b_row * N + col];
    }

    As[threadIdx.y][threadIdx.x] = a_val;
    Bs[threadIdx.y][threadIdx.x] = b_val;

    __syncthreads();

    #pragma unroll
    for (int k = 0; k < TILE; ++k) {
      sum = fmaf(As[threadIdx.y][k], Bs[k][threadIdx.x], sum);
    }

    __syncthreads();
  }

  if (row < N && col < N) {
    C[row * N + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(TILE, TILE);
  const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
