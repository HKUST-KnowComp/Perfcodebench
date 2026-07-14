#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
    __shared__ float As[TILE][TILE];
    __shared__ float Bs[TILE][TILE];

    int row = blockIdx.y * TILE + threadIdx.y;
    int col = blockIdx.x * TILE + threadIdx.x;

    float sum = 0.0f;

    for (int t = 0; t < n; t += TILE) {
        // Load A tile
        if (row < n && (t + threadIdx.x) < n) {
            As[threadIdx.y][threadIdx.x] = A[row * n + (t + threadIdx.x)];
        } else {
            As[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load B tile
        if ((t + threadIdx.y) < n && col < n) {
            Bs[threadIdx.y][threadIdx.x] = B[(t + threadIdx.y) * n + col];
        } else {
            Bs[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        C[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int i = 0; i < iters; ++i) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}