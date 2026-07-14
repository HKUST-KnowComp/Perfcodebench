#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

namespace {

// TILE_M == 128, TILE_N == 8, TILE_K == 8
// Each thread computes 8 outputs; 128x8 threads per CTA.
// Shared mem: double buffer A[2*TILE_M*TILE_K] and B[2*TILE_N*TILE_K]
constexpr int TILE_M = 128;
constexpr int TILE_N = 8;
constexpr int TILE_K = 8;

__global__ void gemm_tiled_f32_kernel(const float* A, const float* B, float* C, int n) {
    const int tx = threadIdx.x;  // 0..127
    const int ty = threadIdx.y;  // 0..7
    const int row0 = blockIdx.y * TILE_M;  // start row of C tile
    const int col0 = blockIdx.x * TILE_N;  // start col of C tile

    // 8 accumulators per thread
    float fragC[8];
    #pragma unroll
    for (int i = 0; i < 8; ++i) fragC[i] = 0.0f;

    // shared memory double buffer
    extern __shared__ float shmem[];
    float* shA = shmem;  // size 2*TILE_M*TILE_K
    float* shB = shmem + 2 * TILE_M * TILE_K; // size 2*TILE_N*TILE_K

    int stage = 0;
    // preload first tile
    // load A tile: TILE_M rows x TILE_K cols, row0..row0+TILE_M-1, k=0..TILE_K-1
    #pragma unroll
    for (int i = 0; i < TILE_M; i += 64) {
        int r = i + tx;
        if (r < TILE_M) {
            const int globalRow = row0 + r;
            if (globalRow < n) {
                #pragma unroll
                for (int kk = 0; kk < TILE_K; kk += 4) {
                    // vectorized load
                    float4 v = *reinterpret_cast<const float4*>(&A[(globalRow * n) + kk]);
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 0] = v.x;
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 1] = v.y;
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 2] = v.z;
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 3] = v.w;
                }
            } else {
                #pragma unroll
                for (int kk = 0; kk < TILE_K; kk += 4) {
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 0] = 0.0f;
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 1] = 0.0f;
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 2] = 0.0f;
                    shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 3] = 0.0f;
                }
            }
        }
    }
    // load B tile: TILE_K rows x TILE_N cols, k=0..TILE_K-1, col0..col0+TILE_N-1
    #pragma unroll
    for (int k = 0; k < TILE_K; k += 8) {
        int kk = k + ty;
        if (kk < TILE_K) {
            const int globalK = kk;
            #pragma unroll
            for (int j = 0; j < TILE_N; j += 8) {
                int jj = j + (tx & 7);
                if (jj < TILE_N) {
                    const int globalCol = col0 + jj;
                    const float val = (globalK < n && globalCol < n) ? B[globalK * n + globalCol] : 0.0f;
                    shB[(stage * TILE_N * TILE_K) + (jj * TILE_K) + kk] = val;
                }
            }
        }
    }

    __syncthreads();

    const int numTiles = (n + TILE_K - 1) / TILE_K;
    for (int tileK = 0; tileK < numTiles; ++tileK) {
        const int nextK = (tileK + 1) * TILE_K;
        // compute current tile
        #pragma unroll
        for (int k = 0; k < TILE_K; ++k) {
            // broadcast A elements (128 threads along M)
            float aTile[TILE_M];
            #pragma unroll
            for (int i = 0; i < TILE_M; i += 32) {
                int ii = i + (tx & 31);
                if (ii < TILE_M) {
                    aTile[ii] = shA[(stage * TILE_M * TILE_K) + (ii * TILE_K) + k];
                }
            }
            __syncwarp();

            // load B vector once per k
            float bVec[TILE_N];
            #pragma unroll
            for (int j = 0; j < TILE_N; j += 8) {
                int jj = j + (tx & 7);
                if (jj < TILE_N) {
                    bVec[jj] = shB[(stage * TILE_N * TILE_K) + (jj * TILE_K) + k];
                }
            }
            __syncwarp();

            // compute 8 outputs per thread
            #pragma unroll
            for (int ii = 0; ii < TILE_M; ii += 1) {
                float aVal = aTile[ii];
                if (ii >= TILE_M) continue;
                const int outRow = row0 + ii;
                if (outRow >= n) continue;
                #pragma unroll
                for (int jj = 0; jj < TILE_N; jj += 8) {
                    int jjv = jj + (tx & 7);
                    if (jjv >= TILE_N) continue;
                    const int outCol = col0 + jjv;
                    if (outCol >= n) continue;
                    int idx = jj / 8; // 0..0 (we have only 8 cols in tile)
                    fragC[idx] += aVal * bVec[jjv];
                }
            }
        }

        __syncthreads();

        // load next tile
        stage = 1 - stage;
        const int kStartNext = nextK;
        // A next tile
        #pragma unroll
        for (int i = 0; i < TILE_M; i += 64) {
            int r = i + tx;
            if (r < TILE_M) {
                const int globalRow = row0 + r;
                if (globalRow < n && kStartNext < n) {
                    #pragma unroll
                    for (int kk = 0; kk < TILE_K; kk += 4) {
                        const int kGlob = kStartNext + kk;
                        if (kGlob < n) {
                            float4 v = *reinterpret_cast<const float4*>(&A[globalRow * n + kGlob]);
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 0] = v.x;
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 1] = v.y;
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 2] = v.z;
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 3] = v.w;
                        } else {
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 0] = 0.0f;
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 1] = 0.0f;
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 2] = 0.0f;
                            shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 3] = 0.0f;
                        }
                    }
                } else {
                    #pragma unroll
                    for (int kk = 0; kk < TILE_K; kk += 4) {
                        shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 0] = 0.0f;
                        shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 1] = 0.0f;
                        shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 2] = 0.0f;
                        shA[(stage * TILE_M * TILE_K) + (r * TILE_K) + kk + 3] = 0.0f;
                    }
                }
            }
        }
        // B next tile
        #pragma unroll
        for (int k = 0; k < TILE_K; k += 8) {
            int kk = k + ty;
            if (kk < TILE_K) {
                const int globalK = kStartNext + kk;
                #pragma unroll
                for (int j = 0; j < TILE_N; j += 8) {
                    int jj = j + (tx & 7);
                    if (jj < TILE_N) {
                        const int globalCol = col0 + jj;
                        const float val = (globalK < n && globalCol < n) ? B[globalK * n + globalCol] : 0.0f;
                        shB[(stage * TILE_N * TILE_K) + (jj * TILE_K) + kk] = val;
                    }
                }
            }
        }
        __syncthreads();
    }

    // store 8 outputs per thread (only 1 column in tile)
    #pragma unroll
    for (int idx = 0; idx < 1; ++idx) {
        const int jj = (tx & 7);
        const int outCol = col0 + jj;
        if (outCol >= n) continue;
        const float val = fragC[idx];
        // write all rows in the tile in a coalesced way
        for (int ii = ty; ii < TILE_M; ii += blockDim.y) {
            const int outRow = row0 + ii;
            if (outRow < n) {
                C[outRow * n + outCol] = fragC[idx];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_M, TILE_N);
    const dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_M - 1) / TILE_M);
    const int shmem = 2 * (TILE_M * TILE_K + TILE_N * TILE_K) * sizeof(float);
    for (int iter = 0; iter < iters; ++iter) {
        gemm_tiled_f32_kernel<<<grid, block, shmem>>>(a, b, c, n);
    }
}