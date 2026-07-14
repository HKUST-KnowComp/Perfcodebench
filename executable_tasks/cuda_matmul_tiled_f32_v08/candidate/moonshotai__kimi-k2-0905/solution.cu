#include "interface.h"
#include <cuda_runtime.h>

namespace {

// 32×8 thread block: 256 threads, good occupancy on modern GPUs
// TILE_K is chosen to fit shared memory and give enough reuse
constexpr int TILE_M = 32;
constexpr int TILE_N = 8;
constexpr int TILE_K = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n)
{
    // Each thread computes 4 elements of C (8×8 tile, 32×8 threads)
    int tx = threadIdx.x;  // 0..31
    int ty = threadIdx.y;  // 0..7

    int rowA = blockIdx.y * TILE_M + tx;      // global row for A
    int colB = blockIdx.x * TILE_N + ty;      // global col for B

    // Shared memory for tiles of A and B
    __shared__ float tileA[TILE_M][TILE_K+1]; // +1 to avoid bank conflicts
    __shared__ float tileB[TILE_K][TILE_N+1];

    float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    int numTiles = (n + TILE_K - 1) / TILE_K;
    for (int t = 0; t < numTiles; ++t) {
        int kBase = t * TILE_K;

        // Load tileA: 32×32 elements, 256 threads -> each loads 4
        #pragma unroll
        for (int i = 0; i < 4; ++i) {
            int globalRow = rowA;
            int globalCol = kBase + (ty * 4 + i);
            if (globalRow < n && globalCol < n) {
                tileA[tx][ty * 4 + i] = A[globalRow * n + globalCol];
            } else {
                tileA[tx][ty * 4 + i] = 0.0f;
            }
        }

        // Load tileB: 32×8 elements, 256 threads -> each loads 1
        #pragma unroll
        for (int i = 0; i < 4; ++i) {
            int globalRow = kBase + (tx / 8) * 4 + i;
            int globalCol = colB + (tx & 7);
            if (globalRow < n && globalCol < n) {
                tileB[(tx / 8) * 4 + i][tx & 7] = B[globalRow * n + globalCol];
            } else {
                tileB[(tx / 8) * 4 + i][tx & 7] = 0.0f;
            }
        }

        __syncthreads();

        // Compute partial dot-product for 4 C elements
        #pragma unroll
        for (int k = 0; k < TILE_K; ++k) {
            float aVec[4];
            #pragma unroll
            for (int i = 0; i < 4; ++i) {
                aVec[i] = tileA[tx][ty * 4 + i];
            }
            float bVal = tileB[k][ty];
            #pragma unroll
            for (int i = 0; i < 4; ++i) {
                sum[i] += aVec[i] * bVal;
            }
        }

        __syncthreads();
    }

    // Write back 4 elements
    #pragma unroll
    for (int i = 0; i < 4; ++i) {
        int globalRow = rowA;
        int globalCol = colB + i;
        if (globalRow < n && globalCol < n) {
            C[globalRow * n + globalCol] = sum[i];
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 blockDim(32, 8);
    dim3 gridDim((n + 8 - 1) / 8, (n + 32 - 1) / 32);
    for (int i = 0; i < iters; ++i) {
        matmul_tiled_kernel<<<gridDim, blockDim>>>(a, b, c, n);
    }
}