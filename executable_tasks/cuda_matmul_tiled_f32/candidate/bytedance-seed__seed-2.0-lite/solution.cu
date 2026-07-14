#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_WIDTH = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float As[TILE_WIDTH][TILE_WIDTH];
    __shared__ float Bs[TILE_WIDTH][TILE_WIDTH];

    const int row = blockIdx.y * TILE_WIDTH + threadIdx.y;
    const int col = blockIdx.x * TILE_WIDTH + threadIdx.x;
    float sum = 0.0f;

    const int num_tiles = (n + TILE_WIDTH - 1) / TILE_WIDTH;
    for (int t = 0; t < num_tiles; ++t) {
        // Load A tile into shared memory
        const int a_col = t * TILE_WIDTH + threadIdx.x;
        As[threadIdx.y][threadIdx.x] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
        // Load B tile into shared memory
        const int b_row = t * TILE_WIDTH + threadIdx.y;
        Bs[threadIdx.y][threadIdx.x] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;
        __syncthreads();

        // Accumulate partial sum from the current tile
        for (int k = 0; k < TILE_WIDTH; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}
} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_WIDTH, TILE_WIDTH);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}