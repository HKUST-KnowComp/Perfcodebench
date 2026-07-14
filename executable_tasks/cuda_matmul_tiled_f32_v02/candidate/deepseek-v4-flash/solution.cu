#include "interface.h"

#include <cuda_runtime.h>

namespace {

const int TILE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float As[TILE][TILE];
    __shared__ float Bs[TILE][TILE];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * TILE + ty;
    int col = bx * TILE + tx;

    float sum = 0.0f;
    for (int k = 0; k < n; k += TILE) {
        if (row < n && k + tx < n)
            As[ty][tx] = a[row * n + (k + tx)];
        else
            As[ty][tx] = 0.0f;
        if (k + ty < n && col < n)
            Bs[ty][tx] = b[(k + ty) * n + col];
        else
            Bs[ty][tx] = 0.0f;
        __syncthreads();

        for (int i = 0; i < TILE; ++i)
            sum += As[ty][i] * Bs[i][tx];
        __syncthreads();
    }

    if (row < n && col < n)
        c[row * n + col] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE, TILE);
    const dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
