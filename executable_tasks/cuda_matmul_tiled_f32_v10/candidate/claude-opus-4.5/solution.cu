#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BM = 64;   // tile rows of C
constexpr int BN = 64;   // tile cols of C
constexpr int BK = 16;   // tile depth
constexpr int TM = 8;    // thread tile rows
constexpr int TN = 8;    // thread tile cols

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
    __shared__ float As[BM][BK];
    __shared__ float Bs[BK][BN];

    const int bx = blockIdx.x;
    const int by = blockIdx.y;
    const int tx = threadIdx.x;  // 0..7
    const int ty = threadIdx.y;  // 0..7

    const int rowC = by * BM + ty * TM;
    const int colC = bx * BN + tx * TN;

    float accum[TM][TN];
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            accum[i][j] = 0.0f;
        }
    }

    const int tid = ty * 8 + tx;  // 0..63

    const int numTiles = (n + BK - 1) / BK;

    for (int t = 0; t < numTiles; ++t) {
        const int kStart = t * BK;

        // Load As: 64 rows x 16 cols = 1024 elements, 64 threads => 16 each
        #pragma unroll
        for (int i = 0; i < 16; ++i) {
            int idx = tid * 16 + i;          // 0..1023
            int aRow = idx / BK;             // 0..63
            int aCol = idx % BK;             // 0..15
            int globalRow = by * BM + aRow;
            int globalCol = kStart + aCol;
            if (globalRow < n && globalCol < n) {
                As[aRow][aCol] = A[globalRow * n + globalCol];
            } else {
                As[aRow][aCol] = 0.0f;
            }
        }

        // Load Bs: 16 rows x 64 cols = 1024 elements, 64 threads => 16 each
        #pragma unroll
        for (int i = 0; i < 16; ++i) {
            int idx = tid * 16 + i;          // 0..1023
            int bRow = idx / BN;             // 0..15
            int bCol = idx % BN;             // 0..63
            int globalRow = kStart + bRow;
            int globalCol = bx * BN + bCol;
            if (globalRow < n && globalCol < n) {
                Bs[bRow][bCol] = B[globalRow * n + globalCol];
            } else {
                Bs[bRow][bCol] = 0.0f;
            }
        }

        __syncthreads();

        // Compute
        #pragma unroll
        for (int k = 0; k < BK; ++k) {
            float aReg[TM];
            float bReg[TN];
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                aReg[i] = As[ty * TM + i][k];
            }
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                bReg[j] = Bs[k][tx * TN + j];
            }
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                #pragma unroll
                for (int j = 0; j < TN; ++j) {
                    accum[i][j] += aReg[i] * bReg[j];
                }
            }
        }

        __syncthreads();
    }

    // Store results
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            int globalRow = rowC + i;
            int globalCol = colC + j;
            if (globalRow < n && globalCol < n) {
                C[globalRow * n + globalCol] = accum[i][j];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(8, 8);  // 64 threads
    dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
