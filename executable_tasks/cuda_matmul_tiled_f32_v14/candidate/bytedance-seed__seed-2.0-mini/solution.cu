#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row = blockIdx.y * BLOCK_SIZE + ty;
    const int col = blockIdx.x * BLOCK_SIZE + tx;

    __shared__ float tile_a[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float tile_b[BLOCK_SIZE][BLOCK_SIZE];

    float sum = 0.0f;

    for (int t = 0; t < n; t += BLOCK_SIZE) {
        // Load tile from matrix A with bounds checking
        if (row < n && (t + tx) < n) {
            tile_a[ty][tx] = a[row * n + t + tx];
        } else {
            tile_a[ty][tx] = 0.0f;
        }

        // Load tile from matrix B with bounds checking
        if ((t + ty) < n && col < n) {
            tile_b[ty][tx] = b[(t + ty) * n + col];
        } else {
            tile_b[ty][tx] = 0.0f;
        }

        __syncthreads();

        // Unroll the dot product loop to eliminate overhead
        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += tile_a[ty][k] * tile_b[k][tx];
        }

        __syncthreads();
    }

    // Write result back to global memory only if within matrix bounds
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