#include "interface.h"
#include <cuda_runtime.h>

#define TILE_DIM 16

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float As[TILE_DIM][TILE_DIM + 1];
    __shared__ float Bs[TILE_DIM][TILE_DIM + 1];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * TILE_DIM + ty;
    int col = bx * TILE_DIM + tx;

    float sum = 0.0f;
    int num_tiles = (n + TILE_DIM - 1) / TILE_DIM;

    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * TILE_DIM + tx;
        int b_row = t * TILE_DIM + ty;

        if (row < n && a_col < n)
            As[ty][tx] = a[row * n + a_col];
        else
            As[ty][tx] = 0.0f;

        if (b_row < n && col < n)
            Bs[ty][tx] = b[b_row * n + col];
        else
            Bs[ty][tx] = 0.0f;

        __syncthreads();

        for (int k = 0; k < TILE_DIM; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
