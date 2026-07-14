#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 16;
constexpr int PADDED_SIZE = BLOCK_SIZE + 1;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[BLOCK_SIZE][PADDED_SIZE];
    __shared__ float Bs[BLOCK_SIZE][PADDED_SIZE];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * BLOCK_SIZE + ty;
    int col = bx * BLOCK_SIZE + tx;

    float sum = 0.0f;
    int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * BLOCK_SIZE + tx;
        int b_row = t * BLOCK_SIZE + ty;

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

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
