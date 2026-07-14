#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_WIDTH = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[TILE_WIDTH][TILE_WIDTH];
    __shared__ float Bs[TILE_WIDTH][TILE_WIDTH];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * TILE_WIDTH + ty;
    const int col = blockIdx.x * TILE_WIDTH + tx;

    float sum = 0.0f;
    const int num_tiles = (n + TILE_WIDTH - 1) / TILE_WIDTH;

    for (int m = 0; m < num_tiles; ++m) {
        // Load A tile into shared memory with boundary handling
        const int a_col = m * TILE_WIDTH + tx;
        As[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;

        // Load B tile into shared memory with boundary handling
        const int b_row = m * TILE_WIDTH + ty;
        Bs[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

        __syncthreads();

        // Accumulate partial sum from shared tiles
        for (int k = 0; k < TILE_WIDTH; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();
    }

    // Write final result to global memory
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