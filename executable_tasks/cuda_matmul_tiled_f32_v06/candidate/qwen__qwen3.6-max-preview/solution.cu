#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_DIM = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, 
                                    const float* __restrict__ b, 
                                    float* __restrict__ c, 
                                    int n) {
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int row = blockIdx.y * BLOCK_DIM + ty;
    const int col = blockIdx.x * BLOCK_DIM + tx;

    __shared__ float sA[BLOCK_DIM][BLOCK_DIM];
    __shared__ float sB[BLOCK_DIM][BLOCK_DIM];

    float sum = 0.0f;
    const int num_tiles = (n + BLOCK_DIM - 1) / BLOCK_DIM;

    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * BLOCK_DIM + tx;
        int b_row = t * BLOCK_DIM + ty;

        sA[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
        sB[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < BLOCK_DIM; ++k) {
            sum += sA[ty][k] * sB[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_DIM, BLOCK_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
