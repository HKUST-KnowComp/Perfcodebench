#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_DIM = 16;

__global__ void matmul_tiled_kernel(const float* __restrict__ A, 
                                    const float* __restrict__ B, 
                                    float* __restrict__ C, int n) {
    __shared__ float As[BLOCK_DIM][BLOCK_DIM + 1];
    __shared__ float Bs[BLOCK_DIM][BLOCK_DIM + 1];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * BLOCK_DIM + ty;
    int col = bx * BLOCK_DIM + tx;

    float sum = 0.0f;
    int num_tiles = (n + BLOCK_DIM - 1) / BLOCK_DIM;

    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * BLOCK_DIM + tx;
        if (row < n && a_col < n) {
            As[ty][tx] = A[row * n + a_col];
        } else {
            As[ty][tx] = 0.0f;
        }

        int b_row = t * BLOCK_DIM + ty;
        if (b_row < n && col < n) {
            Bs[ty][tx] = B[b_row * n + col];
        } else {
            Bs[ty][tx] = 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < BLOCK_DIM; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        if (t < num_tiles - 1) {
            __syncthreads();
        }
    }

    if (row < n && col < n) {
        C[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_DIM, BLOCK_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
