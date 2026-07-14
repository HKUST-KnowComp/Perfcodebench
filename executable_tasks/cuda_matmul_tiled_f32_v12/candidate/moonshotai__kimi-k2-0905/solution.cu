#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BM = 128;          // rows of A tile & C tile
constexpr int BN = 128;          // cols of B tile & C tile
constexpr int BK = 8;            // reduction dimension tile
constexpr int TM = 8;            // rows each thread computes
constexpr int TN = 8;            // cols each thread computes
constexpr int THREADS = (BM * BN) / (TM * TN);

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
  __shared__ float tileA[BM * BK];
  __shared__ float tileB[BK * BN];

  const int tid = threadIdx.x;
  const int rowC = blockIdx.y * BM;
  const int colC = blockIdx.x * BN;

  float regC[TM * TN] = {0.0f};

  for (int k0 = 0; k0 < n; k0 += BK) {
    // Load tileA (BM x BK) with vectorized float4
    #pragma unroll
    for (int i = 0; i < (BM * BK) / 4; i += THREADS) {
      int idx = i + tid;
      if (idx < (BM * BK) / 4) {
        int r = (idx * 4) / BK;
        int c = (idx * 4) % BK;
        int gRow = rowC + r;
        int gCol = k0 + c;
        if (gRow < n && gCol < n) {
          float4 v = *reinterpret_cast<const float4*>(&A[gRow * n + gCol]);
          *reinterpret_cast<float4*>(&tileA[idx * 4]) = v;
        }
      }
    }
    // Load tileB (BK x BN) with vectorized float4
    #pragma unroll
    for (int i = 0; i < (BK * BN) / 4; i += THREADS) {
      int idx = i + tid;
      if (idx < (BK * BN) / 4) {
        int r = (idx * 4) / BN;
        int c = (idx * 4) % BN;
        int gRow = k0 + r;
        int gCol = colC + c;
        if (gRow < n && gCol < n) {
          float4 v = *reinterpret_cast<const float4*>(&B[gRow * n + gCol]);
          *reinterpret_cast<float4*>(&tileB[idx * 4]) = v;
        }
      }
    }
    __syncthreads();

    // Compute
    #pragma unroll
    for (int k = 0; k < BK; ++k) {
      #pragma unroll
      for (int m = 0; m < TM; ++m) {
        float a = tileA[(threadIdx.y * TM + m) * BK + k];
        #pragma unroll
        for (int n = 0; n < TN; ++n) {
          float b = tileB[k * BN + threadIdx.x * TN + n];
          regC[m * TN + n] += a * b;
        }
      }
    }
    __syncthreads();
  }

  // Store C tile with vectorized float4
  #pragma unroll
  for (int m = 0; m < TM; ++m) {
    #pragma unroll
    for (int n = 0; n < TN; n += 4) {
      int gRow = rowC + threadIdx.y * TM + m;
      int gCol = colC + threadIdx.x * TN + n;
      if (gRow < n && gCol + 3 < n) {
        float4 v = {regC[m * TN + n],
                    regC[m * TN + n + 1],
                    regC[m * TN + n + 2],
                    regC[m * TN + n + 3]};
        *reinterpret_cast<float4*>(&C[gRow * n + gCol]) = v;
      }
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 blockDim(THREADS);
  dim3 gridDim((n + BN - 1) / BN, (n + BM - 1) / BM);
  for (int i = 0; i < iters; ++i) {
    matmul_tiled_kernel<<<gridDim, blockDim>>>(a, b, c, n);
  }
}