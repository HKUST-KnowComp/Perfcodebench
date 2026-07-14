#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    const int TILE_SIZE = 16;
    __shared__ float tileA[TILE_SIZE][TILE_SIZE];
    __shared__ float tileB[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    float sum = 0.0f;

    for (int m = 0; m < (n + TILE_SIZE - 1) / TILE_SIZE; ++m) {
        if (row < n && (m * TILE_SIZE + threadIdx.x) < n)
            tileA[threadIdx.y][threadIdx.x] = a[row * n + (m * TILE_SIZE + threadIdx.x)];
        else
            tileA[threadIdx.y][threadIdx.x] = 0.0f;

        if (col < n && (m * TILE_SIZE + threadIdx.y) < n)
            tileB[threadIdx.y][threadIdx.x] = b[(m * TILE_SIZE + threadIdx.y) * n + col];
        else
            tileB[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const int TILE_SIZE = 16;
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}