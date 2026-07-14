#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    constexpr int BLOCK_SIZE = 32;
    __shared__ float s_a[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float s_b[BLOCK_SIZE][BLOCK_SIZE + 1]; // Pad to avoid bank conflicts

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * BLOCK_SIZE + ty;
    const int col = blockIdx.x * BLOCK_SIZE + tx;

    float sum = 0.0f;
    const int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int tile_k = 0; tile_k < num_tiles; ++tile_k) {
        // Load A tile into shared memory
        const int a_col = tile_k * BLOCK_SIZE + tx;
        if (row < n && a_col < n) {
            s_a[ty][tx] = a[row * n + a_col];
        } else {
            s_a[ty][tx] = 0.0f;
        }
        // Load B tile into shared memory
        const int b_row = tile_k * BLOCK_SIZE + ty;
        if (b_row < n && col < n) {
            s_b[ty][tx] = b[b_row * n + col];
        } else {
            s_b[ty][tx] = 0.0f;
        }
        // Wait for all threads to load their tile elements
        __syncthreads();

        // Compute partial sum for this tile
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += s_a[ty][k] * s_b[k][tx];
        }
        // Wait for all threads to finish using shared memory before next tile load
        __syncthreads();
    }

    // Write final result to global memory
    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    constexpr int BLOCK_SIZE = 32;
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}