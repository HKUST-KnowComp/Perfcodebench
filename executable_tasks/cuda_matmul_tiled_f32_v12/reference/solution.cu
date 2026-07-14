#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTile = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float tile_a[kTile][kTile];
  __shared__ float tile_b[kTile][kTile];

  const int row = blockIdx.y * kTile + threadIdx.y;
  const int col = blockIdx.x * kTile + threadIdx.x;
  float sum = 0.0f;

  for (int tile = 0; tile < n; tile += kTile) {
    tile_a[threadIdx.y][threadIdx.x] =
        (row < n && tile + threadIdx.x < n) ? a[row * n + tile + threadIdx.x] : 0.0f;
    tile_b[threadIdx.y][threadIdx.x] =
        (col < n && tile + threadIdx.y < n) ? b[(tile + threadIdx.y) * n + col] : 0.0f;
    __syncthreads();
#pragma unroll
    for (int k = 0; k < kTile; ++k) {
      sum += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
    }
    __syncthreads();
  }

  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(kTile, kTile);
  const dim3 grid((n + kTile - 1) / kTile, (n + kTile - 1) / kTile);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
