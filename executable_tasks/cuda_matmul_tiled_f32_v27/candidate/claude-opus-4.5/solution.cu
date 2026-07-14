#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 64;
constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                     const float* __restrict__ b,
                                     float* __restrict__ c,
                                     int n) {
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int bx = blockIdx.x;
    const int by = blockIdx.y;

    const int row0 = by * TILE_SIZE + ty;
    const int row1 = row0 + BLOCK_SIZE;
    const int col0 = bx * TILE_SIZE + tx;
    const int col1 = col0 + BLOCK_SIZE;

    float c00 = 0.0f, c01 = 0.0f, c10 = 0.0f, c11 = 0.0f;

    const int numTiles = (n + TILE_SIZE - 1) / TILE_SIZE;

    for (int t = 0; t < numTiles; ++t) {
        const int tileStart = t * TILE_SIZE;

        int aCol0 = tileStart + tx;
        int aCol1 = tileStart + tx + BLOCK_SIZE;
        As[ty][tx] = (row0 < n && aCol0 < n) ? a[row0 * n + aCol0] : 0.0f;
        As[ty][tx + BLOCK_SIZE] = (row0 < n && aCol1 < n) ? a[row0 * n + aCol1] : 0.0f;
        As[ty + BLOCK_SIZE][tx] = (row1 < n && aCol0 < n) ? a[row1 * n + aCol0] : 0.0f;
        As[ty + BLOCK_SIZE][tx + BLOCK_SIZE] = (row1 < n && aCol1 < n) ? a[row1 * n + aCol1] : 0.0f;

        int bRow0 = tileStart + ty;
        int bRow1 = tileStart + ty + BLOCK_SIZE;
        Bs[ty][tx] = (bRow0 < n && col0 < n) ? b[bRow0 * n + col0] : 0.0f;
        Bs[ty][tx + BLOCK_SIZE] = (bRow0 < n && col1 < n) ? b[bRow0 * n + col1] : 0.0f;
        Bs[ty + BLOCK_SIZE][tx] = (bRow1 < n && col0 < n) ? b[bRow1 * n + col0] : 0.0f;
        Bs[ty + BLOCK_SIZE][tx + BLOCK_SIZE] = (bRow1 < n && col1 < n) ? b[bRow1 * n + col1] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            float a0 = As[ty][k];
            float a1 = As[ty + BLOCK_SIZE][k];
            float b0 = Bs[k][tx];
            float b1 = Bs[k][tx + BLOCK_SIZE];
            c00 += a0 * b0;
            c01 += a0 * b1;
            c10 += a1 * b0;
            c11 += a1 * b1;
        }

        __syncthreads();
    }

    if (row0 < n && col0 < n) c[row0 * n + col0] = c00;
    if (row0 < n && col1 < n) c[row0 * n + col1] = c01;
    if (row1 < n && col0 < n) c[row1 * n + col0] = c10;
    if (row1 < n && col1 < n) c[row1 * n + col1] = c11;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}