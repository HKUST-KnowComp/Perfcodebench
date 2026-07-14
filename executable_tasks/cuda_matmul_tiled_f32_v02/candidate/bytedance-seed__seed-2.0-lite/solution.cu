#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_WIDTH = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    // Shared memory tiles with padding to avoid CUDA shared memory bank conflicts
    __shared__ float As[TILE_WIDTH][TILE_WIDTH + 1];
    __shared__ float Bs[TILE_WIDTH][TILE_WIDTH + 1];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    // Global row and column for the C element this thread computes
    const int row = blockIdx.y * TILE_WIDTH + ty;
    const int col = blockIdx.x * TILE_WIDTH + tx;

    float sum = 0.0f;
    const int num_tiles = (n + TILE_WIDTH - 1) / TILE_WIDTH;

    for (int t = 0; t < num_tiles; ++t) {
        // Load A tile from global to shared memory, guard against out-of-bounds access
        const int a_global_col = t * TILE_WIDTH + tx;
        if (row < n && a_global_col < n) {
            As[ty][tx] = a[row * n + a_global_col];
        } else {
            As[ty][tx] = 0.0f;
        }

        // Load B tile from global to shared memory
        const int b_global_row = t * TILE_WIDTH + ty;
        if (b_global_row < n && col < n) {
            Bs[ty][tx] = b[b_global_row * n + col];
        } else {
            Bs[ty][tx] = 0.0f;
        }

        // Synchronize to ensure all threads have loaded their tile elements
        __syncthreads();

        // Compute partial dot product for this tile
        for (int k = 0; k < TILE_WIDTH; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        // Synchronize before overwriting shared memory in next iteration
        __syncthreads();
    }

    // Write final result to global memory if in bounds
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