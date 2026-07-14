#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int N) {
    // Shared memory for tiles
    __shared__ float tileA[TILE_SIZE][TILE_SIZE];
    __shared__ float tileB[TILE_SIZE][TILE_SIZE];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Global row and column for C element computed by this thread
    int row = by * TILE_SIZE + ty;
    int col = bx * TILE_SIZE + tx;

    float sum = 0.0f;

    // Number of tiles in the K dimension
    int numTiles = (N + TILE_SIZE - 1) / TILE_SIZE;

    for (int tile = 0; tile < numTiles; ++tile) {
        // Load tile of A: row = by*TILE_SIZE+ty, col = tile*TILE_SIZE+tx
        int aRow = row;
        int aCol = tile * TILE_SIZE + tx;
        if (aRow < N && aCol < N)
            tileA[ty][tx] = A[aRow * N + aCol];
        else
            tileA[ty][tx] = 0.0f;

        // Load tile of B: row = tile*TILE_SIZE+ty, col = bx*TILE_SIZE+tx
        int bRow = tile * TILE_SIZE + ty;
        int bCol = col;
        if (bRow < N && bCol < N)
            tileB[ty][tx] = B[bRow * N + bCol];
        else
            tileB[ty][tx] = 0.0f;

        __syncthreads();

        // Compute partial product for this tile
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += tileA[ty][k] * tileB[k][tx];
        }

        __syncthreads();
    }

    // Store result
    if (row < N && col < N) {
        C[row * N + col] = sum;
    }
}

} // anonymous namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE_SIZE, TILE_SIZE);
    dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE,
              (n + TILE_SIZE - 1) / TILE_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
