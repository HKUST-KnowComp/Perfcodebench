#include "interface.h"
#include <cuda_runtime.h>

namespace {

template <int BLOCK_SIZE>
__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[BLOCK_SIZE][BLOCK_SIZE + 1];
    __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE + 1];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * BLOCK_SIZE + ty;
    int col = bx * BLOCK_SIZE + tx;

    float sum = 0.0f;
    int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int m = 0; m < num_tiles; ++m) {
        int a_col = m * BLOCK_SIZE + tx;
        int b_row = m * BLOCK_SIZE + ty;

        As[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
        Bs[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
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
    constexpr int BLOCK_SIZE = 16;
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<BLOCK_SIZE><<<grid, block>>>(a, b, c, n);
    }
}
