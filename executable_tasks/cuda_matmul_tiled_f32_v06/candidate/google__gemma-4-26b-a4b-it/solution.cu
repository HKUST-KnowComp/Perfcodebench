#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Using a tile size of 32 for better occupancy and memory reuse
    const int TILE_SIZE = 32;
    
    // Shared memory for tiles of A and B
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    const int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    float sum = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load tile from A into shared memory
        if (row < n && (t * TILE_SIZE + threadIdx.x) < n) {
            s_a[threadIdx.y][threadIdx.x] = a[row * n + (t * TILE_SIZE + threadIdx.x)];
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load tile from B into shared memory
        if (col < n && (t * TILE_SIZE + threadIdx.y) < n) {
            s_b[threadIdx.y][threadIdx.x] = b[(t * TILE_SIZE + threadIdx.y) * n + col];
        } else {
            s_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        // Compute partial product for this tile
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

// A more optimized version using 16x16 blocks to match the original grid logic but with tiling
__global__ void matmul_optimized_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    const int TILE_SIZE = 16;
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    const int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    float sum = 0.0f;

    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load A
        int a_row = row;
        int a_col = t * TILE_SIZE + threadIdx.x;
        if (a_row < n && a_col < n) {
            s_a[threadIdx.y][threadIdx.x] = a[a_row * n + a_col];
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load B
        int b_row = t * TILE_SIZE + threadIdx.y;
        int b_col = col;
        if (b_row < n && b_col < n) {
            s_b[threadIdx.y][threadIdx.x] = b[b_row * n + b_col];
        } else {
            s_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
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
    const int TILE_SIZE = 16;
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
    
    for (int iter = 0; iter < iters; ++iter) {
        matmul_optimized_kernel<<<grid, block>>>(a, b, c, n);
    }
}