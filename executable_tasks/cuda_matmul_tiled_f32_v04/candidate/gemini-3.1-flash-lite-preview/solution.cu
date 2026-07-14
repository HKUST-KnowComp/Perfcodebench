#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float tile_a[TILE_SIZE][TILE_SIZE];
    __shared__ float tile_b[TILE_SIZE][TILE_SIZE];

    int bx = blockIdx.x * TILE_SIZE;
    int by = blockIdx.y * TILE_SIZE;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by + ty;
    int col = bx + tx;

    float sum = 0.0f;

    for (int k_tile = 0; k_tile < n; k_tile += TILE_SIZE) {
        if (row < n && (k_tile + tx) < n)
            tile_a[ty][tx] = a[row * n + (k_tile + tx)];
        else
            tile_a[ty][tx] = 0.0f;

        if (col < n && (k_tile + ty) < n)
            tile_b[ty][tx] = b[(k_tile + ty) * n + col];
        else
            tile_b[ty][tx] = 0.0f;

        __syncthreads();

        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += tile_a[ty][k] * tile_b[k][tx];
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