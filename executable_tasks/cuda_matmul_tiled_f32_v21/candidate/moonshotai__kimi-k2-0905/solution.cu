#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;          // width of shared-memory tile
constexpr int TM   = 8;           // rows each thread computes
constexpr int TN   = 8;           // cols each thread computes

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float*       __restrict__ C,
                                    int n)
{
    // Each thread block computes a 32×32 tile of C
    const int by = blockIdx.y;
    const int bx = blockIdx.x;
    const int ty = threadIdx.y;   // 0..7
    const int tx = threadIdx.x;   // 0..31

    // Allocate shared memory for tiles of A and B
    __shared__ float tileA[TILE][TILE];
    __shared__ float tileB[TILE][TILE];

    // Thread-local accumulators
    float accum[TM][TN] = {};

    const int rowStart = by * TILE + ty * TM;   // global row for this thread
    const int colStart = bx * TILE + tx;        // global col for this thread

    const int numTiles = (n + TILE - 1) / TILE;

    for (int t = 0; t < numTiles; ++t) {
        // Load tile of A into shared memory (coalesced)
        #pragma unroll
        for (int m = 0; m < TM; ++m) {
            int globalRow = rowStart + m;
            int globalCol = t * TILE + tx;
            tileA[ty * TM + m][tx] = (globalRow < n && globalCol < n)
                                     ? A[globalRow * n + globalCol]
                                     : 0.0f;
        }

        // Load tile of B into shared memory (coalesced)
        #pragma unroll
        for (int nIdx = 0; nIdx < TN; ++nIdx) {
            int globalRow = t * TILE + ty;
            int globalCol = colStart + nIdx * (TILE / TN);
            tileB[ty][tx + nIdx * (TILE / TN)] = (globalRow < n && globalCol < n)
                                                 ? B[globalRow * n + globalCol]
                                                 : 0.0f;
        }
        __syncthreads();

        // Compute partial dot-product for this tile
        #pragma unroll
        for (int k = 0; k < TILE; ++k) {
            #pragma unroll
            for (int m = 0; m < TM; ++m) {
                float aVal = tileA[ty * TM + m][k];
                #pragma unroll
                for (int nIdx = 0; nIdx < TN; ++nIdx) {
                    accum[m][nIdx] += aVal * tileB[k][tx + nIdx * (TILE / TN)];
                }
            }
        }
        __syncthreads();
    }

    // Write back results
    #pragma unroll
    for (int m = 0; m < TM; ++m) {
        int globalRow = rowStart + m;
        if (globalRow >= n) continue;
        #pragma unroll
        for (int nIdx = 0; nIdx < TN; ++nIdx) {
            int globalCol = colStart + nIdx * (TILE / TN);
            if (globalCol < n) {
                C[globalRow * n + globalCol] = accum[m][nIdx];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE, TILE / TN);   // 32×4 threads
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

    for (int i = 0; i < iters; ++i) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}