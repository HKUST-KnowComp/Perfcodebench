#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tiled matrix multiplication kernel
// Uses shared memory to cache tiles of A and B
template <int BLOCK_SIZE>
__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float tileA[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float tileB[BLOCK_SIZE][BLOCK_SIZE];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * BLOCK_SIZE + ty;
    int col = bx * BLOCK_SIZE + tx;

    float sum = 0.0f;

    for (int m = 0; m < (n + BLOCK_SIZE - 1) / BLOCK_SIZE; ++m) {
        // Load tileA
        if (row < n && (m * BLOCK_SIZE + tx) < n)
            tileA[ty][tx] = a[row * n + (m * BLOCK_SIZE + tx)];
        else
            tileA[ty][tx] = 0.0f;

        // Load tileB
        if (col < n && (m * BLOCK_SIZE + ty) < n)
            tileB[ty][tx] = b[(m * BLOCK_SIZE + ty) * n + col];
        else
            tileB[ty][tx] = 0.0f;

        __syncthreads();

        // Compute partial dot product
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += tileA[ty][k] * tileB[k][tx];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const int BLOCK_SIZE = 32;
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<BLOCK_SIZE><<<grid, block>>>(a, b, c, n);
    }
}