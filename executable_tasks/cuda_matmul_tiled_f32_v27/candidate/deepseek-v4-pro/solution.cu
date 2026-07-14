#include "interface.h"

#include <cuda_runtime.h>

namespace {

#define TILE_DIM 32

__global__ void matmul_tiled_kernel(const float* __restrict__ A, const float* __restrict__ B, float* __restrict__ C, int N) {
    __shared__ float As[TILE_DIM][TILE_DIM];
    __shared__ float Bs[TILE_DIM][TILE_DIM];
    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;
    int row = by * TILE_DIM + ty;
    int col = bx * TILE_DIM + tx;
    float sum = 0.0f;
    for (int k = 0; k < N; k += TILE_DIM) {
        // Load A tile into shared memory (coalesced)
        if (row < N && (k + tx) < N) {
            As[ty][tx] = A[row * N + k + tx];
        } else {
            As[ty][tx] = 0.0f;
        }
        // Load B tile into shared memory (coalesced)
        if (col < N && (k + ty) < N) {
            Bs[ty][tx] = B[(k + ty) * N + col];
        } else {
            Bs[ty][tx] = 0.0f;
        }
        __syncthreads();
        // Unrolled dot product
        #pragma unroll
        for (int i = 0; i < TILE_DIM; ++i) {
            sum += As[ty][i] * Bs[i][tx];
        }
        __syncthreads();
    }
    if (row < N && col < N) {
        C[row * N + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
