#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float a_tile[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float b_tile[BLOCK_SIZE][BLOCK_SIZE];

    int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
    int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;

    float sum = 0.0f;
    int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * BLOCK_SIZE + threadIdx.x;
        int b_row = t * BLOCK_SIZE + threadIdx.y;

        if (row < n && a_col < n)
            a_tile[threadIdx.y][threadIdx.x] = a[row * n + a_col];
        else
            a_tile[threadIdx.y][threadIdx.x] = 0.0f;

        if (b_row < n && col < n)
            b_tile[threadIdx.y][threadIdx.x] = b[b_row * n + col];
        else
            b_tile[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += a_tile[threadIdx.y][k] * b_tile[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    dim3 grid((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
