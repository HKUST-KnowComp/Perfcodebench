#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
    __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];

    const int threadRow = threadIdx.y;
    const int threadCol = threadIdx.x;
    const int globalRow = blockIdx.y * BLOCK_SIZE + threadRow;
    const int globalCol = blockIdx.x * BLOCK_SIZE + threadCol;

    float sum = 0.0f;

    const int numTiles = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int t = 0; t < numTiles; t++) {
        // Load A tile into shared memory with bounds checking
        const int aCol = t * BLOCK_SIZE + threadCol;
        if (globalRow < n && aCol < n) {
            As[threadRow][threadCol] = a[globalRow * n + aCol];
        } else {
            As[threadRow][threadCol] = 0.0f;
        }

        // Load B tile into shared memory with bounds checking
        const int bRow = t * BLOCK_SIZE + threadRow;
        if (bRow < n && globalCol < n) {
            Bs[threadRow][threadCol] = b[bRow * n + globalCol];
        } else {
            Bs[threadRow][threadCol] = 0.0f;
        }

        // Synchronize to ensure all tiles are loaded before computation
        __syncthreads();

        // Compute partial sum for this tile
        for (int k = 0; k < BLOCK_SIZE; k++) {
            sum += As[threadRow][k] * Bs[k][threadCol];
        }

        // Synchronize before loading next tile to avoid race conditions
        __syncthreads();
    }

    // Write final result to global memory
    if (globalRow < n && globalCol < n) {
        c[globalRow * n + globalCol] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; iter++) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}