#include "interface.h"

#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 16

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
    __shared__ float sA[TILE_SIZE][TILE_SIZE];
    __shared__ float sB[TILE_SIZE][TILE_SIZE];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * TILE_SIZE + ty;
    int col = bx * TILE_SIZE + tx;

    float sum = 0.0f;

    // Iterate over tiles of A (columns) and B (rows)
    int numTiles = (n + TILE_SIZE - 1) / TILE_SIZE;
    for (int tileIdx = 0; tileIdx < numTiles; ++tileIdx) {
        // Load tile from A into shared memory
        int aCol = tileIdx * TILE_SIZE + tx;
        if (row < n && aCol < n) {
            sA[ty][tx] = a[row * n + aCol];
        } else {
            sA[ty][tx] = 0.0f;
        }

        // Load tile from B into shared memory
        int bRow = tileIdx * TILE_SIZE + ty;
        if (bRow < n && col < n) {
            sB[ty][tx] = b[bRow * n + col];
        } else {
            sB[ty][tx] = 0.0f;
        }

        __syncthreads();

        // Compute partial dot product
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += sA[ty][k] * sB[k][tx];
        }

        __syncthreads();
    }

    // Write the final result if within bounds
    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE_SIZE, TILE_SIZE);
    dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE,
              (n + TILE_SIZE - 1) / TILE_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}