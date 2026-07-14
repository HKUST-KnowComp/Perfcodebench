#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A, 
                                    const float* __restrict__ B, 
                                    float* __restrict__ C, int n) {
    __shared__ float As[TILE_DIM][TILE_DIM];
    __shared__ float Bs[TILE_DIM][TILE_DIM + 1];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * TILE_DIM + ty;
    int col = bx * TILE_DIM + tx;

    float sum = 0.0f;

    int num_tiles = (n + TILE_DIM - 1) / TILE_DIM;
    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * TILE_DIM + tx;
        int b_row = t * TILE_DIM + ty;

        As[ty][tx] = (row < n && a_col < n) ? A[row * n + a_col] : 0.0f;
        Bs[tx][ty] = (b_row < n && col < n) ? B[b_row * n + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_DIM; ++k) {
            sum += As[ty][k] * Bs[tx][k];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        C[row * n + col] = sum;
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
