#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_M = 64;
constexpr int TILE_N = 64;
constexpr int TILE_K = 16;
constexpr int TM = 8;
constexpr int TN = 8;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int n) {
    __shared__ float As[TILE_M][TILE_K];
    __shared__ float Bs[TILE_K][TILE_N];

    const int bx = blockIdx.x;
    const int by = blockIdx.y;

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    constexpr int THREADS_X = TILE_N / TN;
    constexpr int THREADS_Y = TILE_M / TM;
    constexpr int BLOCK_SIZE = THREADS_X * THREADS_Y;

    const int tid = ty * THREADS_X + tx;

    const int rowC = by * TILE_M + ty * TM;
    const int colC = bx * TILE_N + tx * TN;

    float accum[TM][TN];
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            accum[i][j] = 0.0f;
        }
    }

    constexpr int A_TILE_ELEMS = TILE_M * TILE_K;
    constexpr int B_TILE_ELEMS = TILE_K * TILE_N;
    constexpr int A_LOADS_PER_THREAD = (A_TILE_ELEMS + BLOCK_SIZE - 1) / BLOCK_SIZE;
    constexpr int B_LOADS_PER_THREAD = (B_TILE_ELEMS + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int k0 = 0; k0 < n; k0 += TILE_K) {
        #pragma unroll
        for (int l = 0; l < A_LOADS_PER_THREAD; ++l) {
            int idx = tid + l * BLOCK_SIZE;
            if (idx < A_TILE_ELEMS) {
                int ar = idx / TILE_K;
                int ac = idx % TILE_K;
                int globalRow = by * TILE_M + ar;
                int globalCol = k0 + ac;
                if (globalRow < n && globalCol < n) {
                    As[ar][ac] = A[globalRow * n + globalCol];
                } else {
                    As[ar][ac] = 0.0f;
                }
            }
        }

        #pragma unroll
        for (int l = 0; l < B_LOADS_PER_THREAD; ++l) {
            int idx = tid + l * BLOCK_SIZE;
            if (idx < B_TILE_ELEMS) {
                int br = idx / TILE_N;
                int bc = idx % TILE_N;
                int globalRow = k0 + br;
                int globalCol = bx * TILE_N + bc;
                if (globalRow < n && globalCol < n) {
                    Bs[br][bc] = B[globalRow * n + globalCol];
                } else {
                    Bs[br][bc] = 0.0f;
                }
            }
        }

        __syncthreads();

        #pragma unroll
        for (int kk = 0; kk < TILE_K; ++kk) {
            float a_reg[TM];
            float b_reg[TN];
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                a_reg[i] = As[ty * TM + i][kk];
            }
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                b_reg[j] = Bs[kk][tx * TN + j];
            }
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                #pragma unroll
                for (int j = 0; j < TN; ++j) {
                    accum[i][j] += a_reg[i] * b_reg[j];
                }
            }
        }

        __syncthreads();
    }

    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        #pragma unroll
        for (int j = 0; j < TN; ++j) {
            int globalRow = rowC + i;
            int globalCol = colC + j;
            if (globalRow < n && globalCol < n) {
                C[globalRow * n + globalCol] = accum[i][j];
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    constexpr int THREADS_X = TILE_N / TN;
    constexpr int THREADS_Y = TILE_M / TM;
    dim3 block(THREADS_X, THREADS_Y);
    dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_M - 1) / TILE_M);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
