#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Tuned tile sizes for FP32 GEMM on a broad range of GPUs.
constexpr int BM = 64;
constexpr int BN = 64;
constexpr int BK = 16;
constexpr int TM = 4;
constexpr int TN = 4;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
  __shared__ float As[BM][BK];
  __shared__ float Bs[BK][BN];

  const int tx = threadIdx.x; // 0..15
  const int ty = threadIdx.y; // 0..15

  const int block_row = blockIdx.y * BM;
  const int block_col = blockIdx.x * BN;

  const int row_base = block_row + ty * TM;
  const int col_base = block_col + tx * TN;

  float acc[TM][TN];
#pragma unroll
  for (int i = 0; i < TM; ++i) {
#pragma unroll
    for (int j = 0; j < TN; ++j) {
      acc[i][j] = 0.0f;
    }
  }

  for (int kb = 0; kb < n; kb += BK) {
    // Cooperative load of A tile: 64x16 = 1024 elements, 256 threads * 4 loads.
#pragma unroll
    for (int i = 0; i < TM; ++i) {
      const int a_row = block_row + ty * TM + i;
      const int a_col = kb + tx;
      As[ty * TM + i][tx] = (a_row < n && a_col < n) ? a[a_row * n + a_col] : 0.0f;
    }

    // Cooperative load of B tile: 16x64 = 1024 elements, 256 threads * 4 loads.
#pragma unroll
    for (int j = 0; j < TN; ++j) {
      const int b_row = kb + ty;
      const int b_col = block_col + tx * TN + j;
      Bs[ty][tx * TN + j] = (b_row < n && b_col < n) ? b[b_row * n + b_col] : 0.0f;
    }

    __syncthreads();

#pragma unroll
    for (int k = 0; k < BK; ++k) {
      float a_frag[TM];
      float b_frag[TN];

#pragma unroll
      for (int i = 0; i < TM; ++i) {
        a_frag[i] = As[ty * TM + i][k];
      }
#pragma unroll
      for (int j = 0; j < TN; ++j) {
        b_frag[j] = Bs[k][tx * TN + j];
      }

#pragma unroll
      for (int i = 0; i < TM; ++i) {
#pragma unroll
        for (int j = 0; j < TN; ++j) {
          acc[i][j] += a_frag[i] * b_frag[j];
        }
      }
    }

    __syncthreads();
  }

#pragma unroll
  for (int i = 0; i < TM; ++i) {
    const int row = row_base + i;
    if (row < n) {
#pragma unroll
      for (int j = 0; j < TN; ++j) {
        const int col = col_base + j;
        if (col < n) {
          c[row * n + col] = acc[i][j];
        }
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  const dim3 block(16, 16);
  const dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);

  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}
