#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                     const float* __restrict__ b,
                                     float* __restrict__ c,
                                     int n) {
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * TILE_SIZE + ty;
    const int col = blockIdx.x * TILE_SIZE + tx;

    float sum = 0.0f;

    const int numTiles = (n + TILE_SIZE - 1) / TILE_SIZE;

    for (int t = 0; t < numTiles; ++t) {
        const int aCol = t * TILE_SIZE + tx;
        const int bRow = t * TILE_SIZE + ty;

        // Load tile from A into shared memory
        if (row < n && aCol < n) {
            As[ty][tx] = a[row * n + aCol];
        } else {
            As[ty][tx] = 0.0f;
        }

        // Load tile from B into shared memory
        if (bRow < n && col < n) {
            Bs[ty][tx] = b[bRow * n + col];
        } else {
            Bs[ty][tx] = 0.0f;
        }

        __syncthreads();

        // Compute partial product for this tile
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
