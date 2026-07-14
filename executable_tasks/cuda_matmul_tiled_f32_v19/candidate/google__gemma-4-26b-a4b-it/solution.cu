#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float tile_a[TILE_SIZE][TILE_SIZE];
    __shared__ float tile_b[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;
    float sum = 0.0f;

    for (int m = 0; m < (n + TILE_SIZE - 1) / TILE_SIZE; ++m) {
        // Load tile from A into shared memory
        if (row < n && (m * TILE_SIZE + threadIdx.x) < n) {
            tile_a[threadIdx.y][threadIdx.x] = a[row * n + (m * TILE_SIZE + threadIdx.x)];
        } else {
            tile_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load tile from B into shared memory
        if (col < n && (m * TILE_SIZE + threadIdx.y) < n) {
            tile_b[threadIdx.y][threadIdx.x] = b[(m * TILE_SIZE + threadIdx.y) * n + col];
        } else {
            tile_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        // Compute partial product using the tile
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    // Using a 32x32 block size to maximize occupancy and utilize shared memory efficiently
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
    cudaDeviceSynchronize();
}