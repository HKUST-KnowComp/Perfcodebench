#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 32

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    const int col = blockIdx.x * TILE_SIZE + threadIdx.x;
    float sum = 0.0f;

    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load tile from A
        if (row < n && (t * TILE_SIZE + threadIdx.x) < n) {
            s_a[threadIdx.y][threadIdx.x] = a[row * n + t * TILE_SIZE + threadIdx.x];
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load tile from B
        if (col < n && (t * TILE_SIZE + threadIdx.y) < n) {
            s_b[threadIdx.y][threadIdx.x] = b[(t * TILE_SIZE + threadIdx.y) * n + col];
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
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
    
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}