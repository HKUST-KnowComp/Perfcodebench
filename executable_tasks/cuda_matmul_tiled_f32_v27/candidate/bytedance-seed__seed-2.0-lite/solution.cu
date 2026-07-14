#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float As[BLOCK_SIZE][BLOCK_SIZE + 1];
    __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE + 1];

    const int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
    const int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    float sum = 0.0f;

    const int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int tile = 0; tile < num_tiles; ++tile) {
        // Load A tile to shared memory
        const int a_col = tile * BLOCK_SIZE + threadIdx.x;
        if (row < n && a_col < n) {
            As[threadIdx.y][threadIdx.x] = a[row * n + a_col];
        } else {
            As[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load B tile to shared memory
        const int b_row = tile * BLOCK_SIZE + threadIdx.y;
        if (b_row < n && col < n) {
            Bs[threadIdx.y][threadIdx.x] = b[b_row * n + col];
        } else {
            Bs[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        // Accumulate partial sum from current tile
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }

        __syncthreads();
    }

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