#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n, int iters) {
    __shared__ float tileA[TILE_SIZE][TILE_SIZE];
    __shared__ float tileB[TILE_SIZE][TILE_SIZE];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int row = by * TILE_SIZE + ty;
    int col = bx * TILE_SIZE + tx;

    for (int iter = 0; iter < iters; ++iter) {
        float sum = 0.0f;
        for (int k = 0; k < n; k += TILE_SIZE) {
            // Load tile of A
            if (row < n && k + tx < n)
                tileA[ty][tx] = a[row * n + k + tx];
            else
                tileA[ty][tx] = 0.0f;

            // Load tile of B
            if (k + ty < n && col < n)
                tileB[ty][tx] = b[(k + ty) * n + col];
            else
                tileB[ty][tx] = 0.0f;

            __syncthreads();

            #pragma unroll
            for (int i = 0; i < TILE_SIZE; ++i)
                sum += tileA[ty][i] * tileB[i][tx];

            __syncthreads();
        }

        if (row < n && col < n)
            c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE,
                    (n + TILE_SIZE - 1) / TILE_SIZE);
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n, iters);
}
