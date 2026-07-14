#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    const int TILE = 32;  // Must match blockDim.x, blockDim.y
    __shared__ float As[TILE][TILE];
    __shared__ float Bs[TILE][TILE];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * TILE + ty;
    int col = bx * TILE + tx;

    float sum = 0.0f;

    for (int t = 0; t < n; t += TILE) {
        // Load tile from A (row constant across tile inner loop)
        if (row < n && t + tx < n)
            As[ty][tx] = a[row * n + t + tx];
        else
            As[ty][tx] = 0.0f;

        // Load tile from B (col constant across tile inner loop)
        if (t + ty < n && col < n)
            Bs[ty][tx] = b[(t + ty) * n + col];
        else
            Bs[ty][tx] = 0.0f;

        __syncthreads();

        // Compute partial dot product over the current tile
        for (int k = 0; k < TILE; ++k) {
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
    const int TILE = 32;
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}