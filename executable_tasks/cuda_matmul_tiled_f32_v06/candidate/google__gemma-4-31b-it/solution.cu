#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Shared memory for tiles
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int row = blockIdx.y * TILE_SIZE + ty;
    int col = blockIdx.x * TILE_SIZE + tx;

    float sum = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load tile from A into shared memory
        if (row < n && (t * TILE_SIZE + tx) < n) {
            s_a[ty][tx] = a[row * n + (t * TILE_SIZE + tx)];
        } else {
            s_a[ty][tx] = 0.0f;
        }

        // Load tile from B into shared memory
        if (col < n && (t * TILE_SIZE + ty) < n) {
            s_b[ty][tx] = b[(t * TILE_SIZE + ty) * n + col];
        } else {
            s_b[ty][tx] = 0.0f;
        }

        __syncthreads();

        // Compute partial product for this tile
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += s_a[ty][k] * s_b[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}