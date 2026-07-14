#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    // Pad shared memory to avoid bank conflicts
    __shared__ float a_tile[BLOCK_SIZE][BLOCK_SIZE + 1];
    __shared__ float b_tile[BLOCK_SIZE][BLOCK_SIZE + 1];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * BLOCK_SIZE + ty;
    const int col = blockIdx.x * BLOCK_SIZE + tx;

    float sum = 0.0f;
    const int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int tile = 0; tile < num_tiles; ++tile) {
        // Load A tile with bounds checking
        const int a_col = tile * BLOCK_SIZE + tx;
        if (row < n && a_col < n) {
            a_tile[ty][tx] = a[row * n + a_col];
        } else {
            a_tile[ty][tx] = 0.0f;
        }

        // Load B tile with bounds checking
        const int b_row = tile * BLOCK_SIZE + ty;
        if (b_row < n && col < n) {
            b_tile[ty][tx] = b[b_row * n + col];
        } else {
            b_tile[ty][tx] = 0.0f;
        }

        // Wait for all threads to load tile data
        __syncthreads();

        // Compute partial sum for this tile
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += a_tile[ty][k] * b_tile[k][tx];
        }

        // Wait for computation to finish before overwriting shared memory
        __syncthreads();
    }

    // Write final result to global memory
    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}