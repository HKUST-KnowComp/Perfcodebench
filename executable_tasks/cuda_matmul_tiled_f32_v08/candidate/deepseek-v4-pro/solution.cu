#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_DIM 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[TILE_DIM][TILE_DIM];
    __shared__ float Bs[TILE_DIM][TILE_DIM];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * TILE_DIM + ty;
    int col = bx * TILE_DIM + tx;

    float sum = 0.0f;

    for (int p = 0; p < n; p += TILE_DIM) {
        // Load tile of A
        int aRow = row;
        int aCol = p + tx;
        if (aRow < n && aCol < n)
            As[ty][tx] = a[aRow * n + aCol];
        else
            As[ty][tx] = 0.0f;

        // Load tile of B
        int bRow = p + ty;
        int bCol = col;
        if (bRow < n && bCol < n)
            Bs[ty][tx] = b[bRow * n + bCol];
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

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
