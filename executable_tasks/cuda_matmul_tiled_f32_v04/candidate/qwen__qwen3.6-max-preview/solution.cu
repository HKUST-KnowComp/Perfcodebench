#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_DIM = 16;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, 
                                    const float* __restrict__ b, 
                                    float* __restrict__ c, int n) {
    // Pad shared memory by 1 column to avoid bank conflicts during strided access
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
        int b_row = t * BLOCK_DIM + ty;

        // Coalesced global memory loads into shared memory tiles
        As[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
        Bs[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

        __syncthreads();

        // Compute partial dot product for this tile
        #pragma unroll
        for (int k = 0; k < BLOCK_DIM; ++k) {
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
    const dim3 block(BLOCK_DIM, BLOCK_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}