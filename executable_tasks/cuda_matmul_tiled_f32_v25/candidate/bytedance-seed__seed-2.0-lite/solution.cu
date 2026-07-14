#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float s_a[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float s_b[BLOCK_SIZE][BLOCK_SIZE];

    const int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
    const int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    float sum = 0.0f;

    for (int tile_k = 0; tile_k < (n + BLOCK_SIZE - 1) / BLOCK_SIZE; ++tile_k) {
        // Load tiles into shared memory with bounds checking
        if (row < n && (tile_k * BLOCK_SIZE + threadIdx.x) < n) {
            s_a[threadIdx.y][threadIdx.x] = a[row * n + tile_k * BLOCK_SIZE + threadIdx.x];
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }
        if (col < n && (tile_k * BLOCK_SIZE + threadIdx.y) < n) {
            s_b[threadIdx.y][threadIdx.x] = b[(tile_k * BLOCK_SIZE + threadIdx.y) * n + col];
        } else {
            s_b[threadIdx.y][threadIdx.x] = 0.0f;
        }
        __syncthreads();

        // Compute partial sum for this tile
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
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