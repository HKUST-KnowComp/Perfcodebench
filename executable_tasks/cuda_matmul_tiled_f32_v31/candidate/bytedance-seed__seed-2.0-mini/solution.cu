#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row = blockIdx.y * TILE_SIZE + ty;
    const int col = blockIdx.x * TILE_SIZE + tx;

    float sum = 0.0f;

    __shared__ float tile_a[TILE_SIZE][TILE_SIZE + 1];
    __shared__ float tile_b[TILE_SIZE][TILE_SIZE + 1];

    for (int s = 0; s < n; s += TILE_SIZE) {
        if (row < n && (s + tx) < n) {
            tile_a[ty][tx] = a[row * n + (s + tx)];
        } else {
            tile_a[ty][tx] = 0.0f;
        }

        if ((s + ty) < n && col < n) {
            tile_b[ty][tx] = b[(s + ty) * n + col];
        } else {
            tile_b[ty][tx] = 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int t = 0; t < TILE_SIZE; ++t) {
            sum += tile_a[ty][t] * tile_b[t][tx];
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
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}