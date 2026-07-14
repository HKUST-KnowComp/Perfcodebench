#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_DIM 32
#define BLOCK_ROWS 8

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float tileA[TILE_DIM][TILE_DIM];
    __shared__ float tileB[TILE_DIM][TILE_DIM];

    int row = blockIdx.y * TILE_DIM + threadIdx.y;
    int col = blockIdx.x * TILE_DIM + threadIdx.x;

    float sum[4][4] = {0.0f};

    for (int k = 0; k < n; k += TILE_DIM) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (row + i * BLOCK_ROWS < n && k + threadIdx.x < n)
                    tileA[threadIdx.y + i * BLOCK_ROWS][threadIdx.x] = a[(row + i * BLOCK_ROWS) * n + k + threadIdx.x];
                else
                    tileA[threadIdx.y + i * BLOCK_ROWS][threadIdx.x] = 0.0f;

                if (k + threadIdx.y + i * BLOCK_ROWS < n && col + j * (TILE_DIM/4) < n)
                    tileB[threadIdx.y + i * BLOCK_ROWS][threadIdx.x + j * (TILE_DIM/4)] = b[(k + threadIdx.y + i * BLOCK_ROWS) * n + col + j * (TILE_DIM/4)];
                else
                    tileB[threadIdx.y + i * BLOCK_ROWS][threadIdx.x + j * (TILE_DIM/4)] = 0.0f;
            }
        }
        __syncthreads();

        for (int e = 0; e < TILE_DIM; ++e) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    sum[i][j] += tileA[threadIdx.y + i * BLOCK_ROWS][e] * tileB[e][threadIdx.x + j * (TILE_DIM/4)];
                }
            }
        }
        __syncthreads();
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (row + i * BLOCK_ROWS < n && col + j * (TILE_DIM/4) < n)
                c[(row + i * BLOCK_ROWS) * n + col + j * (TILE_DIM/4)] = sum[i][j];
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE_DIM / 4, BLOCK_ROWS);
    dim3 grid((n + TILE_DIM - 1) / TILE_DIM, (n + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}