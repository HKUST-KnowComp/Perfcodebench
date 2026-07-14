#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 32;

template <int BLOCK_SIZE>
__global__ void matmul_tiled(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];

    const int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
    const int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;

    float sum = 0.0f;
    const int num_tiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int t = 0; t < num_tiles; ++t) {
        // Load tile from matrix A with bounds checking
        const int a_col = t * BLOCK_SIZE + threadIdx.x;
        As[threadIdx.y][threadIdx.x] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;

        // Load tile from matrix B with bounds checking
        const int b_row = t * BLOCK_SIZE + threadIdx.y;
        Bs[threadIdx.y][threadIdx.x] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

        __syncthreads();

        // Unrolled dot product accumulation for maximum throughput
        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }

        __syncthreads();
    }

    // Write final result to global memory with bounds checking
    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled<BLOCK_SIZE><<<grid, block>>>(a, b, c, n);
    }
}