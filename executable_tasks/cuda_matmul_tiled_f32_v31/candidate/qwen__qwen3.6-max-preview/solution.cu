#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, 
                                    const float* __restrict__ b, 
                                    float* __restrict__ c, int n) {
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * TILE_SIZE + ty;
    const int col = blockIdx.x * TILE_SIZE + tx;

    float sum = 0.0f;
    const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

    for (int m = 0; m < num_tiles; ++m) {
        const int a_col = m * TILE_SIZE + tx;
        const int b_row = m * TILE_SIZE + ty;

        As[ty][tx] = (row < n && a_col < n) ? a[(size_t)row * n + a_col] : 0.0f;
        Bs[ty][tx] = (b_row < n && col < n) ? b[(size_t)b_row * n + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[(size_t)row * n + col] = sum;
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