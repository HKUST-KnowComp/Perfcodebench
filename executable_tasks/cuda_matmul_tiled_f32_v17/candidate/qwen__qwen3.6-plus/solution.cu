#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float As[TILE_DIM][TILE_DIM];
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

        As[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
        Bs[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;
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
    const dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
