#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BM = 128;
constexpr int BN = 128;
constexpr int BK = 8;
constexpr int TM = 8;
constexpr int TN = 8;

__global__ void matmul_kernel(const float* __restrict__ A,
                              const float* __restrict__ B,
                              float*       __restrict__ C,
                              int n) {
  __shared__ float As[BM * BK];
  __shared__ float Bs[BK * BN];

  int tx = threadIdx.x & 31;            // warp lane
  int ty = threadIdx.x >> 5;            // warp id (0..3)
  int tid = threadIdx.x;

  int rowA = tid >> 2;
  int colA = (tid & 3) << 2;

  int rowB = tid >> 5;
  int colB = (tid & 31);

  int blockRow = blockIdx.y * BM;
  int blockCol = blockIdx.x * BN;

  float accum[TM * TN] = {0.0f};

  for (int k = 0; k < n; k += BK) {
    #pragma unroll
    for (int i = 0; i < 4; ++i) {
      if (blockRow + rowA + i * 32 < n && k + colA + i < n) {
        As[(rowA + i * 32) * BK + colA + i] =
            A[(blockRow + rowA + i * 32) * n + k + colA + i];
      } else {
        As[(rowA + i * 32) * BK + colA + i] = 0.f;
      }
    }

    #pragma unroll
    for (int i = 0; i < 4; ++i) {
      if (k + rowB + i * 32 < n && blockCol + colB < n) {
        Bs[(rowB + i * 32) * BN + colB] =
            B[(k + rowB + i * 32) * n + blockCol + colB];
      } else {
        Bs[(rowB + i * 32) * BN + colB] = 0.0f;
      }
    }
    __syncthreads();

    #pragma unroll
    for (int kk = 0; kk < BK; ++kk) {
      #pragma unroll
      for (int i = 0; i < TM; ++i) {
        float a = As[(ty * TM + i) * BK + kk];
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
          float b = Bs[kk * BN + tx * TN + j];
          accum[i * TN + j] += a * b;
        }
      }
    }
    __syncthreads();
  }

  #pragma unroll
  for (int i = 0; i < TM; ++i) {
    int globalRow = blockRow + ty * TM + i;
    if (globalRow >= n) continue;
    #pragma unroll
    for (int j = 0; j < TN; ++j) {
      int globalCol = blockCol + tx * TN + j;
      if (globalCol >= n) continue;
      C[globalRow * n + globalCol] = accum[i * TN + j];
    }
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  dim3 blockDim(128);
  dim3 gridDim((n + 127) / 128, (n + 127) / 128);
  for (int i = 0; i < iters; ++i) {
    matmul_kernel<<<gridDim, blockDim>>>(a, b, c, n);
  }
}