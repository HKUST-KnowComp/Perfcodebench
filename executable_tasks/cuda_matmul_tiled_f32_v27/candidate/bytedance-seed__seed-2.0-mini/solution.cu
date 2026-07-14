#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;
static_assert(TILE_DIM * TILE_DIM <= 1024, "Block thread count exceeds CUDA maximum (1024)");

__global__ void tiled_matmul_kernel(const float* a, const float* b, float* c, int n) {
    // Shared memory with padding to avoid bank conflicts
    __shared__ float s_a[TILE_DIM][TILE_DIM + 1];
    __shared__ float s_b[TILE_DIM][TILE_DIM + 1];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row = blockIdx.y * TILE_DIM + ty;
    const int col = blockIdx.x * TILE_DIM + tx;

    float sum = 0.0f;

    for (int k_tile = 0; k_tile < n; k_tile += TILE_DIM) {
        // Load tile from matrix A: row x [k_tile, k_tile + TILE_DIM)
        if (row < n && (k_tile + tx) < n) {
            s_a[ty][tx] = a[row * n + (k_tile + tx)];
        } else {
            s_a[ty][tx] = 0.0f;
        }

        // Load tile from matrix B: [k_tile, k_tile + TILE_DIM) x col
        if ((k_tile + ty) < n && col < n) {
            s_b[ty][tx] = b[(k_tile + ty) * n + col];
        } else {
            s_b[ty][tx] = 0.0f;
        }

        // Synchronize to ensure all threads have loaded their shared memory tiles
        __syncthreads();

        // Compute dot product of the current row of A tile and column of B tile
        #pragma unroll
        for (int t = 0; t < TILE_DIM; ++t) {
            sum += s_a[ty][t] * s_b[t][tx];
        }

        // Synchronize to prevent overwriting shared memory before all threads finish using it
        __syncthreads();
    }

    // Write result to global memory if within matrix bounds
    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block_dim(TILE_DIM, TILE_DIM);
    const dim3 grid_dim(
        (n + block_dim.x - 1) / block_dim.x,
        (n + block_dim.y - 1) / block_dim.y
    );

    for (int iter = 0; iter < iters; ++iter) {
        tiled_matmul_kernel<<<grid_dim, block_dim>>>(a, b, c, n);
    }
}