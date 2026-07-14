#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;               // shared-memory tile size
constexpr int TMR  = 8;                // per-thread tile (rows & cols)

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float*       __restrict__ C,
                                    int n)
{
    // 16×16 thread block → 256 threads
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    // Global tile coordinates
    const int tileRow = blockIdx.y * TILE;
    const int tileCol = blockIdx.x * TILE;

    // Shared memory tiles with padding to avoid bank conflicts
    __shared__ float As[TILE][TILE + 1];
    __shared__ float Bs[TILE][TILE + 1];

    // Each thread computes an 8×8 sub-tile of C
    float c[TMR][TMR] = {};

    const int innerTiles = (n + TILE - 1) / TILE;
    for (int t = 0; t < innerTiles; ++t) {
        // Load tile of A into shared memory
        #pragma unroll
        for (int i = 0; i < TILE; i += 16) {
            int globalRow = tileRow + i + ty;
            int globalCol = t * TILE + tx;
            As[i + ty][tx] = (globalRow < n && globalCol < n) ?
                               A[globalRow * n + globalCol] : 0.0f;
        }
        // Load tile of B into shared memory
        #pragma unroll
        for (int i = 0; i < TILE; i += 16) {
            int globalRow = t * TILE + ty;
            int globalCol = tileCol + i + tx;
            Bs[ty][i + tx] = (globalRow < n && globalCol < n) ?
                               B[globalRow * n + globalCol] : 0.0f;
        }
        __syncthreads();

        // Compute partial products
        #pragma unroll
        for (int k = 0; k < TILE; ++k) {
            #pragma unroll
            for (int tr = 0; tr < TMR; ++tr) {
                #pragma unroll
                for (int tc = 0; tc < TMR; ++tc) {
                    c[tr][tc] += As[ty * TMR + tr][k] *
                                 Bs[k][tx * TMR + tc];
                }
            }
        }
        __syncthreads();
    }

    // Write 8×8 results back to global memory
    #pragma unroll
    for (int tr = 0; tr < TMR; ++tr) {
        #pragma unroll
        for (int tc = 0; tc < TMR; ++tc) {
            int globalRow = tileRow + ty * TMR + tr;
            int globalCol = tileCol + tx * TMR + tc;
            if (globalRow < n && globalCol < n) {
                C[globalRow * n + globalCol] = c[tr][tc];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(16, 16);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int i = 0; i < iters; ++i) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}