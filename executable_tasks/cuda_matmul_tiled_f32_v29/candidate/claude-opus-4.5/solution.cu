#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 64;
constexpr int BLOCK_SIZE = 16;
constexpr int THREAD_TILE = 4;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                     const float* __restrict__ b,
                                     float* __restrict__ c,
                                     int n) {
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int bx = blockIdx.x;
    const int by = blockIdx.y;

    const int row_base = by * TILE_SIZE + ty * THREAD_TILE;
    const int col_base = bx * TILE_SIZE + tx * THREAD_TILE;

    float acc[THREAD_TILE][THREAD_TILE] = {{0.0f}};

    const int num_tiles = (n + TILE_SIZE - 1) / TILE_SIZE;

    for (int t = 0; t < num_tiles; ++t) {
        const int tile_start = t * TILE_SIZE;

        #pragma unroll
        for (int i = 0; i < THREAD_TILE; ++i) {
            #pragma unroll
            for (int j = 0; j < THREAD_TILE; ++j) {
                int local_row = ty * THREAD_TILE + i;
                int local_col = tx * THREAD_TILE + j;
                int global_row = by * TILE_SIZE + local_row;
                int global_col = tile_start + local_col;
                if (global_row < n && global_col < n) {
                    As[local_row][local_col] = a[global_row * n + global_col];
                } else {
                    As[local_row][local_col] = 0.0f;
                }
            }
        }

        #pragma unroll
        for (int i = 0; i < THREAD_TILE; ++i) {
            #pragma unroll
            for (int j = 0; j < THREAD_TILE; ++j) {
                int local_row = ty * THREAD_TILE + i;
                int local_col = tx * THREAD_TILE + j;
                int global_row = tile_start + local_row;
                int global_col = bx * TILE_SIZE + local_col;
                if (global_row < n && global_col < n) {
                    Bs[local_row][local_col] = b[global_row * n + global_col];
                } else {
                    Bs[local_row][local_col] = 0.0f;
                }
            }
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            float a_reg[THREAD_TILE];
            float b_reg[THREAD_TILE];

            #pragma unroll
            for (int i = 0; i < THREAD_TILE; ++i) {
                a_reg[i] = As[ty * THREAD_TILE + i][k];
            }

            #pragma unroll
            for (int j = 0; j < THREAD_TILE; ++j) {
                b_reg[j] = Bs[k][tx * THREAD_TILE + j];
            }

            #pragma unroll
            for (int i = 0; i < THREAD_TILE; ++i) {
                #pragma unroll
                for (int j = 0; j < THREAD_TILE; ++j) {
                    acc[i][j] += a_reg[i] * b_reg[j];
                }
            }
        }

        __syncthreads();
    }

    #pragma unroll
    for (int i = 0; i < THREAD_TILE; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_TILE; ++j) {
            int global_row = row_base + i;
            int global_col = col_base + j;
            if (global_row < n && global_col < n) {
                c[global_row * n + global_col] = acc[i][j];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
