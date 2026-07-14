#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    const int col = blockIdx.x * TILE_SIZE + threadIdx.x;
    float sum = 0.0f;

    const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;
    for (int k_tile = 0; k_tile < num_tiles; ++k_tile) {
        // Load A tile into shared memory
        const int a_col = k_tile * TILE_SIZE + threadIdx.x;
        if (row < n && a_col < n) {
            s_a[threadIdx.y][threadIdx.x] = __ldg(&a[row * n + a_col]);
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load B tile into shared memory
        const int b_row = k_tile * TILE_SIZE + threadIdx.y;
        if (b_row < n && col < n) {
            s_b[threadIdx.y][threadIdx.x] = __ldg(&b[b_row * n + col]);
        } else {
            s_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        // Accumulate partial sum from current tiles
        #pragma unroll 32
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}