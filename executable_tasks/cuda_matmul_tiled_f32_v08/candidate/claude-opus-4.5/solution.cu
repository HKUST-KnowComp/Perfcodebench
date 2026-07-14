#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BM = 64;   // tile rows of C
constexpr int BN = 64;   // tile cols of C
constexpr int BK = 64;   // tile depth
constexpr int TM = 8;    // sub-tile rows per thread
constexpr int TN = 8;    // sub-tile cols per thread

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
    const int tid = ty * 8 + tx; // 0..63

    const int cRow = by * BM;
    const int cCol = bx * BN;

    float accum[TM][TN];
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            accum[i][j] = 0.0f;
        }
    }

    const int numTiles = (n + BK - 1) / BK;

    for (int t = 0; t < numTiles; ++t) {
        const int tileK = t * BK;

        // Load A tile: BM x BK = 64x64 = 4096 elements, 64 threads, 64 each
        #pragma unroll
        for (int i = 0; i < 64; ++i) {
            int idx = tid * 64 + i;
            int r = idx / BK;
            int c = idx % BK;
            int globalRow = cRow + r;
            int globalCol = tileK + c;
            As[r][c] = (globalRow < n && globalCol < n) ? A[globalRow * n + globalCol] : 0.0f;
        }

        // Load B tile: BK x BN = 64x64 = 4096 elements
        #pragma unroll
        for (int i = 0; i < 64; ++i) {
            int idx = tid * 64 + i;
            int r = idx / BN;
            int c = idx % BN;
            int globalRow = tileK + r;
            int globalCol = cCol + c;
            Bs[r][c] = (globalRow < n && globalCol < n) ? B[globalRow * n + globalCol] : 0.0f;
        }

        __syncthreads();

        // Compute: each thread handles TM x TN sub-tile
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

    // Write results
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            int globalRow = cRow + ty * TM + i;
            int globalCol = cCol + tx * TN + j;
            if (globalRow < n && globalCol < n) {
                C[globalRow * n + globalCol] = accum[i][j];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(8, 8);  // 64 threads per block
    dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
