#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BM = 128;   // tile height for A/C
constexpr int BN = 8;     // tile width  for B/C
constexpr int BK = 8;     // reduction dimension tile size

constexpr int TM = 2;     // per-thread rows in A tile
constexpr int TN = 2;     // per-thread cols in B tile

__global__ void gemm_tiled(const float* __restrict__ A,
                           const float* __restrict__ B,
                           float*       __restrict__ C,
                           int n) {
  __shared__ float tileA[BM * BK];
  __shared__ float tileB[BK * BN];

  const int tx = threadIdx.x;
  const int ty = threadIdx.y;
  const int bx = blockIdx.x;
  const int by = blockIdx.y;

  const int rowA = ty * TM + (tx / 4) * TM;  // 128 threads -> 128/8 = 16 warps -> 8x16 threads
  const int colA = (tx & 3) * 2;

  const int rowB = tx / BN;
  const int colB = tx % BN;

  const int globalRowStart = by * BM;
  const int globalColStart = bx * BN;

  float accum[TM][TN] = {{0.0f}};

  for (int bk = 0; bk < n; bk += BK) {
    /* Load tileA (BM x BK) with vectorized float4 */
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
      int row = rowA + i;
      if (row < BM && globalRowStart + row < n) {
        const float4* src = reinterpret_cast<const float4*>(&A[(globalRowStart + row) * n + bk + colA]);
        float4 val = *src;
        tileA[(row) * BK + colA + 0] = val.x;
        tileA[(row) * BK + colA + 1] = val.y;
        if (colA + 2 < BK)
          tileA[(row) * BK + colA + 2] = val.z;
        if (colA + 3 < BK)
          tileA[(row) * BK + colA + 3] = val.w;
      }
    }

    /* Load tileB (BK x BN) with vectorized float4 */
    #pragma unroll
    for (int i = 0; i < 2; ++i) {
      int row = rowB + i * 64;
      if (row < BK && bk + row < n) {
        float4 val = *reinterpret_cast<const float4*>(&B[(bk + row) * n + globalColStart + colB]);
        tileB[(row) * BN + colB + 0] = val.x;
        tileB[(row) * BN + colB + 1] = val.y;
        tileB[(row) * BN + colB + 2] = val.z;
        tileB[(row) * BN + colB + 3] = val.w;
      }
    }

    __syncthreads();

    /* Compute */
    #pragma unroll
    for (int k = 0; k < BK; ++k) {
      #pragma unroll
      for (int m = 0; m < TM; ++m) {
        #pragma unroll
        for (int n = 0; n < TN; ++n) {
          accum[m][n] += tileA[(ty * TM + m) * BK + k] *
                         tileB[k * BN + tx * TN + n];
        }
      }
    }
    __syncthreads();
  }

  /* Store C tile */
  #pragma unroll
  for (int m = 0; m < TM; ++m) {
    #pragma unroll
    for (int n = 0; n < TN; ++n) {
      int globalRow = globalRowStart + ty * TM + m;
      int globalCol = globalColStart + tx * TN + n;
      if (globalRow < n && globalCol < n) {
        C[globalRow * n + globalCol] = accum[m][n];
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 blockDim(32, 8);                 // 256 threads
  dim3 gridDim((n + BN - 1) / BN,
               (n + BM - 1) / BM);

  for (int iter = 0; iter < iters; ++iter) {
    gemm_tiled<<<gridDim, blockDim>>>(a, b, c, n);
  }
}
