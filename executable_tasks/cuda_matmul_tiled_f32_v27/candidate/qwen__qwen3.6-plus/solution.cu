#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;
constexpr int PAD = 1;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[TILE_DIM][TILE_DIM + PAD];
    __shared__ float Bs[TILE_DIM][TILE_DIM + PAD];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * TILE_DIM + ty;
    int col = bx * TILE_DIM + tx;

    float sum = 0.0f;

    int num_tiles = (n + TILE_DIM - 1) / TILE_DIM;

    for (int m = 0; m < num_tiles; ++m) {
        int a_col = m * TILE_DIM + tx;
        int b_row = m * TILE_DIM + ty;

        if (row < n && a_col < n)
            As[ty][tx] = a[row * n + a_col];
        else
            As[ty][tx] = 0.0f;

        if (b_row < n && col < n)
            Bs[ty][tx] = b[b_row * n + col];
        else
            Bs[ty][tx] = 0.0f;

        __syncthreads();

        #pragma unroll
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
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
