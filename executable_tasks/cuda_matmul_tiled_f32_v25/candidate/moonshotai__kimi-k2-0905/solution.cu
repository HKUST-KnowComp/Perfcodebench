#include "interface.h"
#include <cuda_runtime.h>

namespace {

// 32x32 tile, 256 threads (16x16), each thread computes 8 C elements
constexpr int TILE = 32;
constexpr int THREADS = 16;

__global__ void matmul_tile_kernel(const float* __restrict__ A,
                                   const float* __restrict__ B,
                                   float* __restrict__ C,
                                   int n) {
    // Shared memory for 32x32 tiles of A and B
    __shared__ float tileA[TILE][TILE + 1]; // +1 to avoid bank conflicts
    __shared__ float tileB[TILE][TILE + 1];

    int by = blockIdx.y, bx = blockIdx.x;
    int ty = threadIdx.y, tx = threadIdx.x;

    int row = by * TILE + ty;
    int col = bx * TILE + tx;

    // Each thread accumulates 8 elements in its register file
    float sum[8] = {0.0f};

    int numTiles = (n + TILE - 1) / TILE;
    for (int t = 0; t < numTiles; ++t) {
        // Load tile of A into shared memory (coalesced)
        #pragma unroll
        for (int i = 0; i < 2; ++i) {
            int globalRow = by * TILE + ty + i * 16;
            int globalCol = t * TILE + tx;
            if (globalRow < n && globalCol < n) {
                tileA[ty + i * 16][tx] = A[globalRow * n + globalCol];
            } else {
                tileA[ty + i * 16][tx] = 0.0f;
            }
        }

        // Load tile of B into shared memory (coalesced)
        #pragma unroll
        for (int i = 0; i < 2; ++i) {
            int globalRow = t * TILE + ty + i * 16;
            int globalCol = bx * TILE + tx;
            if (globalRow < n && globalCol < n) {
                tileB[ty + i * 16][tx] = B[globalRow * n + globalCol];
            } else {
                tileB[ty + i * 16][tx] = 0.0f;
            }
        }
        __syncthreads();

        // Compute partial dot products
        #pragma unroll 4
        for (int k = 0; k < TILE; ++k) {
            float aVec[2] = { tileA[ty][k], tileA[ty + 16][k] };
            float bVec[2] = { tileB[k][tx], tileB[k][tx + 16] };
            sum[0] += aVec[0] * bVec[0];
            sum[1] += aVec[0] * bVec[1];
            sum[2] += aVec[1] * bVec[0];
            sum[3] += aVec[1] * bVec[1];
        }
        __syncthreads();
    }

    // Write back 8 elements
    #pragma unroll
    for (int i = 0; i < 2; ++i) {
        int globalRow = row + i * 16;
        int globalCol = col;
        if (globalRow < n && globalCol < n) {
            C[globalRow * n + globalCol] = sum[i * 4 + 0];
        }
        if (globalRow < n && (globalCol + 16) < n) {
            C[globalRow * n + (globalCol + 16)] = sum[i * 4 + 1];
        }
    }
    #pragma unroll
    for (int i = 0; i < 2; ++i) {
        int globalRow = row + i * 16;
        int globalCol = col;
        if ((globalRow + 16) < n && globalCol < n) {
            C[(globalRow + 16) * n + globalCol] = sum[i * 4 + 2];
        }
        if ((globalRow + 16) < n && (globalCol + 16) < n) {
            C[(globalRow + 16) * n + (globalCol + 16)] = sum[i * 4 + 3];
        }
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(THREADS, THREADS);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int i = 0; i < iters; ++i) {
        matmul_tile_kernel<<<grid, block>>>(a, b, c, n);
    }
}