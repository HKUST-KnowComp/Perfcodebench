#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_WIDTH = 16;

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float As[TILE_WIDTH][TILE_WIDTH];
    __shared__ float Bs[TILE_WIDTH][TILE_WIDTH];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * TILE_WIDTH + ty;
    const int col = blockIdx.x * TILE_WIDTH + tx;

    float sum = 0.0f;

    for (int ph = 0; ph < n; ph += TILE_WIDTH) {
        // Load A tile to shared memory with bounds checking
        if (row < n && (ph + tx) < n) {
            As[ty][tx] = a[row * n + ph + tx];
        } else {
            As[ty][tx] = 0.0f;
        }
        // Load B tile to shared memory with bounds checking
        if ((ph + ty) < n && col < n) {
            Bs[ty][tx] = b[(ph + ty) * n + col];
        } else {
            Bs[ty][tx] = 0.0f;
        }

        __syncthreads();

        // Compute partial sum using shared memory values
        for (int k = 0; k < TILE_WIDTH; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_WIDTH, TILE_WIDTH);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}