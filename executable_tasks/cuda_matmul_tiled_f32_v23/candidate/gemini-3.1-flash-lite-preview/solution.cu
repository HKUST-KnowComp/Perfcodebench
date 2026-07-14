#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_DIM 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float tileA[TILE_DIM][TILE_DIM];
    __shared__ float tileB[TILE_DIM][TILE_DIM];

    int row = blockIdx.y * TILE_DIM + threadIdx.y;
    int col = blockIdx.x * TILE_DIM + threadIdx.x;

    float sum = 0.0f;

    for (int m = 0; m < (n + TILE_DIM - 1) / TILE_DIM; ++m) {
        if (row < n && (m * TILE_DIM + threadIdx.x) < n)
            tileA[threadIdx.y][threadIdx.x] = a[row * n + (m * TILE_DIM + threadIdx.x)];
        else
            tileA[threadIdx.y][threadIdx.x] = 0.0f;

        if (col < n && (m * TILE_DIM + threadIdx.y) < n)
            tileB[threadIdx.y][threadIdx.x] = b[(m * TILE_DIM + threadIdx.y) * n + col];
        else
            tileB[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_DIM; ++k) {
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
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}