#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float a_tile[16][16];
    __shared__ float b_tile[16][16];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * 16 + ty;
    int col = bx * 16 + tx;

    if (row >= n || col >= n) {
        return;
    }

    float sum = 0.0f;
    int num_tiles = (n + 15) / 16;

    for (int k = 0; k < num_tiles; ++k) {
        // Load A tile
        int a_row = by * 16 + ty;
        int a_col = k * 16 + tx;
        if (a_row < n && a_col < n) {
            a_tile[ty][tx] = a[a_row * n + a_col];
        } else {
            a_tile[ty][tx] = 0.0f;
        }

        // Load B tile
        int b_row = k * 16 + ty;
        int b_col = bx * 16 + tx;
        if (b_row < n && b_col < n) {
            b_tile[ty][tx] = b[b_row * n + b_col];
        } else {
            b_tile[ty][tx] = 0.0f;
        }

        __syncthreads();

        // Compute partial sum for this k tile
        for (int k_prime = 0; k_prime < 16; ++k_prime) {
            sum += a_tile[ty][k_prime] * b_tile[k_prime][tx];
        }
    }

    c[row * n + col] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(16, 16);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}