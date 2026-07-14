#include "interface.h"
#include <cuda_runtime.h>

#define TILE_SIZE 16

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float shared_a[TILE_SIZE][TILE_SIZE];
    __shared__ float shared_b[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    if (row >= n || col >= n) {
        return;
    }

    float sum = 0.0f;

    for (int k = 0; k < n; k += TILE_SIZE) {
        int a_row = row;
        int a_col = k + threadIdx.x;
        if (a_row < n && a_col < n) {
            shared_a[threadIdx.y][threadIdx.x] = a[a_row * n + a_col];
        } else {
            shared_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        int b_row = k + threadIdx.y;
        int b_col = col;
        if (b_row < n && b_col < n) {
            shared_b[threadIdx.y][threadIdx.x] = b[b_row * n + b_col];
        } else {
            shared_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        for (int i = 0; i < TILE_SIZE; ++i) {
            sum += shared_a[threadIdx.y][i] * shared_b[i][threadIdx.x];
        }

        __syncthreads();
    }

    c[row * n + col] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}