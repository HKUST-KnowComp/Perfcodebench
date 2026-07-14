#include "interface.h"
#include <cuda_runtime.h>

#define TILE 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
    int row = blockIdx.y * TILE + threadIdx.y;
    int col = blockIdx.x * TILE + threadIdx.x;

    float sum = 0.0f;

    __shared__ float As[TILE][TILE];
    __shared__ float Bs[TILE][TILE];

    for (int tile = 0; tile < n; tile += TILE) {
        // Load A tile
        if (row < n && (tile + threadIdx.x) < n) {
            As[threadIdx.y][threadIdx.x] = a[row * n + tile + threadIdx.x];
        } else {
            As[threadIdx.y][threadIdx.x] = 0.0f;
        }
        // Load B tile
        if ((tile + threadIdx.y) < n && col < n) {
            Bs[threadIdx.y][threadIdx.x] = b[(tile + threadIdx.y) * n + col];
        } else {
            Bs[threadIdx.y][threadIdx.x] = 0.0f;
        }
        __syncthreads();

        // Accumulate
        for (int k = 0; k < TILE; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}