#include "interface.h"

#include <cuda_runtime.h>

namespace {

template <int BLOCK_SIZE>
__global__ void __launch_bounds__(BLOCK_SIZE * BLOCK_SIZE)
matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    __shared__ float tile_a[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float tile_b[BLOCK_SIZE][BLOCK_SIZE];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int row = blockIdx.y * BLOCK_SIZE + ty;
    int col = blockIdx.x * BLOCK_SIZE + tx;

    float sum = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (n + BLOCK_SIZE - 1) / BLOCK_SIZE; ++t) {
        // Collaborative loading of A tile
        if (row < n && (t * BLOCK_SIZE + tx) < n)
            tile_a[ty][tx] = a[row * n + t * BLOCK_SIZE + tx];
        else
            tile_a[ty][tx] = 0.0f;

        // Collaborative loading of B tile
        if ((t * BLOCK_SIZE + ty) < n && col < n)
            tile_b[ty][tx] = b[(t * BLOCK_SIZE + ty) * n + col];
        else
            tile_b[ty][tx] = 0.0f;

        __syncthreads();

        // Compute partial dot product
        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += tile_a[ty][k] * tile_b[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n)
        c[row * n + col] = sum;
}

} // anonymous namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const int BLOCK_SIZE = 32;
    dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    dim3 grid((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<BLOCK_SIZE><<<grid, block>>>(a, b, c, n);
    }
}
