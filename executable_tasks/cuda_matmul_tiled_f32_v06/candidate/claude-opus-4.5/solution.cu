#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE = 64;
constexpr int BLOCK = 16;
constexpr int TM = 4;
constexpr int TN = 4;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
    __shared__ float As[TILE][TILE];
    __shared__ float Bs[TILE][TILE];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row0 = blockIdx.y * TILE + ty * TM;
    const int col0 = blockIdx.x * TILE + tx * TN;

    float acc[TM][TN];
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            acc[i][j] = 0.0f;
        }
    }

    const int numTiles = (n + TILE - 1) / TILE;

    for (int t = 0; t < numTiles; ++t) {
        const int tileStart = t * TILE;

        // Load A tile: each thread loads TM * TN elements spread across the tile
        // Thread (tx, ty) loads elements at rows [ty*TM .. ty*TM+TM) and cols [tx*TN .. tx*TN+TN)
        #pragma unroll
        for (int i = 0; i < TM; ++i) {
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                int lr = ty * TM + i;
                int lc = tx * TN + j;
                int gr = blockIdx.y * TILE + lr;
                int gc = tileStart + lc;
                As[lr][lc] = (gr < n && gc < n) ? a[gr * n + gc] : 0.0f;
            }
        }

        // Load B tile
        #pragma unroll
        for (int i = 0; i < TM; ++i) {
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                int lr = ty * TM + i;
                int lc = tx * TN + j;
                int gr = tileStart + lr;
                int gc = blockIdx.x * TILE + lc;
                Bs[lr][lc] = (gr < n && gc < n) ? b[gr * n + gc] : 0.0f;
            }
        }

        __syncthreads();

        // Compute partial products
        #pragma unroll
        for (int k = 0; k < TILE; ++k) {
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
                    acc[i][j] += aReg[i] * bReg[j];
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
            int gr = row0 + i;
            int gc = col0 + j;
            if (gr < n && gc < n) {
                c[gr * n + gc] = acc[i][j];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(BLOCK, BLOCK);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
