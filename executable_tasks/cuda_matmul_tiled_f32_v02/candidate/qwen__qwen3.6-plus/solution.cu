#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * TILE_SIZE + ty;
    int col = bx * TILE_SIZE + tx;

    float sum = 0.0f;
    int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

    for (int t = 0; t < num_tiles; ++t) {
        if (row < n && (t * TILE_SIZE + tx) < n) {
            s_a[ty][tx] = a[row * n + t * TILE_SIZE + tx];
        } else {
            s_a[ty][tx] = 0.0f;
        }

        if ((t * TILE_SIZE + ty) < n && col < n) {
            s_b[ty][tx] = b[(t * TILE_SIZE + ty) * n + col];
        } else {
            s_b[ty][tx] = 0.0f;
        }

        __syncthreads();

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

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_SIZE, TILE_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
