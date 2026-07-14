#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                     const float* __restrict__ b,
                                     float* __restrict__ c,
                                     int n) {
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    float sum = 0.0f;

    int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;
    for (int t = 0; t < num_tiles; ++t) {
        // Load tile of A
        int a_col = t * TILE_SIZE + threadIdx.x;
        if (row < n && a_col < n)
            As[threadIdx.y][threadIdx.x] = a[row * n + a_col];
        else
            As[threadIdx.y][threadIdx.x] = 0.0f;

        // Load tile of B
        int b_row = t * TILE_SIZE + threadIdx.y;
        if (b_row < n && col < n)
            Bs[threadIdx.y][threadIdx.x] = b[b_row * n + col];
        else
            Bs[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // anonymous namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE_SIZE, TILE_SIZE);
    dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}