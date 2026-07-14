#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_DIM = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[TILE_DIM][TILE_DIM];
    __shared__ float Bs[TILE_DIM][TILE_DIM];

    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;

    for (int kTile = 0; kTile < (n + TILE_DIM - 1) / TILE_DIM; ++kTile) {
        int aRow = row;
        int aCol = kTile * TILE_DIM + threadIdx.x;
        if (aRow < n && aCol < n)
            As[threadIdx.y][threadIdx.x] = a[aRow * n + aCol];
        else
            As[threadIdx.y][threadIdx.x] = 0.0f;

        int bRow = kTile * TILE_DIM + threadIdx.y;
        int bCol = col;
        if (bRow < n && bCol < n)
            Bs[threadIdx.y][threadIdx.x] = b[bRow * n + bCol];
        else
            Bs[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_DIM; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}
} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}