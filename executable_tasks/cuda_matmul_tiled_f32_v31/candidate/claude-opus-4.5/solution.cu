#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 64;
constexpr int TM = 8;
constexpr int TN = 8;
constexpr int BK = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
    __shared__ float As[TILE][BK];
    __shared__ float Bs[BK][TILE];

    const int bx = blockIdx.x;
    const int by = blockIdx.y;

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row0 = by * TILE + ty * TM;
    const int col0 = bx * TILE + tx * TN;

    float acc[TM][TN];
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            acc[i][j] = 0.0f;
        }
    }

    const int numTiles = (n + BK - 1) / BK;

    const int tid = ty * (TILE / TN) + tx;
    const int totalThreads = (TILE / TM) * (TILE / TN);

    for (int t = 0; t < numTiles; ++t) {
        const int kStart = t * BK;

        #pragma unroll
        for (int idx = tid; idx < TILE * BK; idx += totalThreads) {
            int i = idx / BK;
            int j = idx % BK;
            int globalRow = by * TILE + i;
            int globalCol = kStart + j;
            As[i][j] = (globalRow < n && globalCol < n) ? A[globalRow * n + globalCol] : 0.0f;
        }

        #pragma unroll
        for (int idx = tid; idx < BK * TILE; idx += totalThreads) {
            int i = idx / TILE;
            int j = idx % TILE;
            int globalRow = kStart + i;
            int globalCol = bx * TILE + j;
            Bs[i][j] = (globalRow < n && globalCol < n) ? B[globalRow * n + globalCol] : 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < BK; ++k) {
            float a_reg[TM];
            float b_reg[TN];

            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                a_reg[i] = As[ty * TM + i][k];
            }
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                b_reg[j] = Bs[k][tx * TN + j];
            }

            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                #pragma unroll
                for (int j = 0; j < TN; ++j) {
                    acc[i][j] += a_reg[i] * b_reg[j];
                }
            }
        }

        __syncthreads();
    }

    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            int globalRow = row0 + i;
            int globalCol = col0 + j;
            if (globalRow < n && globalCol < n) {
                C[globalRow * n + globalCol] = acc[i][j];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE / TN, TILE / TM);
    const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
