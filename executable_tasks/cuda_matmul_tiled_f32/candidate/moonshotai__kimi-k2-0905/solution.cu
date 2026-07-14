#include "interface.h"
#include <cuda_runtime.h>

namespace {

// 32x32 tile size
constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n)
{
    // 1-D block: TILE*TILE threads
    int tx = threadIdx.x & (TILE - 1);           // col in tile
    int ty = threadIdx.x >> 5;                   // row in tile
    int tileRow = blockIdx.y * TILE + ty;
    int tileCol = blockIdx.x * TILE + tx;

    __shared__ float tileA[TILE][TILE];
    __shared__ float tileB[TILE][TILE];

    float sum = 0.0f;

    for (int t = 0; t < (n + TILE - 1) / TILE; ++t) {
        // Load tileA: A[tileRow][t*TILE + tx]
        int aRow = tileRow;
        int aCol = t * TILE + tx;
        tileA[ty][tx] = (aRow < n && aCol < n) ? A[aRow * n + aCol] : 0.0f;

        // Load tileB: B[t*TILE + ty][tileCol]
        int bRow = t * TILE + ty;
        int bCol = tileCol;
        tileB[ty][tx] = (bRow < n && bCol < n) ? B[bRow * n + bCol] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE; ++k) {
            sum += tileA[ty][k] * tileB[k][tx];
        }
        __syncthreads();
    }

    if (tileRow < n && tileCol < n) {
        C[tileRow * n + tileCol] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE * TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int i = 0; i < iters; ++i) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}