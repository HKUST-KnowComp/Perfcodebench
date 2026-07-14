#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 16;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x * TILE;
    int by = blockIdx.y * TILE;

    int row = by + ty;
    int col = bx + tx;

    __shared__ float As[TILE][TILE];
    __shared__ float Bs[TILE][TILE];

    float sum = 0.0f;
    int numTiles = (n + TILE - 1) / TILE;

    for (int t = 0; t < numTiles; ++t) {
        // Load A tile: A[row][t*TILE + tx]
        int k = t * TILE + tx;
        if (row < n && k < n) {
            As[ty][tx] = a[row * n + k];
        } else {
            As[ty][tx] = 0.0f;
        }

        // Load B tile (transposed into Bs): B[t*TILE + ty][col]
        k = t * TILE + ty;
        if (k < n && col < n) {
            Bs[tx][ty] = b[k * n + col];
        } else {
            Bs[tx][ty] = 0.0f;
        }

        __syncthreads();

        // Compute partial product
        #pragma unroll
        for (int i = 0; i < TILE; ++i) {
            sum += As[ty][i] * Bs[tx][i];
        }

        __syncthreads();
    }

    // Write result
    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
