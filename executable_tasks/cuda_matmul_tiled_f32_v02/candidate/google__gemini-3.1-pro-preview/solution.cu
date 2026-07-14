#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tiling parameters
// Each block handles a 32x32 tile of C.
// Each thread handles a 4x4 sub-tile of C.
// Block size: (32/4) x (32/4) = 8x8 = 64 threads.

template <int TILE_N, int TILE_K, int THREAD_M, int THREAD_N>
__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float sa[TILE_N][TILE_K];
    __shared__ float sb[TILE_K][TILE_N];

    const int ty = threadIdx.y;
    const int tx = threadIdx.x;
    const int row_start = blockIdx.y * TILE_N + ty * THREAD_M;
    const int col_start = blockIdx.x * TILE_N + tx * THREAD_N;

    float res[THREAD_M][THREAD_N];
    #pragma unroll
    for (int i = 0; i < THREAD_M; ++i) {
        #pragma unroll
        for (int j = 0; j < THREAD_N; ++j) {
            res[i][j] = 0.0f;
        }
    }

    for (int k_offset = 0; k_offset < n; k_offset += TILE_K) {
        // Load tiles into shared memory
        // Each thread in the 8x8 block needs to load (32*32)/(8*8) = 16 elements
        #pragma unroll
        for (int i = 0; i < (TILE_N * TILE_K) / (8 * 8); ++i) {
            int tid = ty * 8 + tx;
            int local_id = tid + i * 64;
            int r = local_id / TILE_K;
            int c_idx = local_id % TILE_K;
            int global_row = blockIdx.y * TILE_N + r;
            int global_col = k_offset + c_idx;
            if (global_row < n && global_col < n)
                sa[r][c_idx] = a[global_row * n + global_col];
            else
                sa[r][c_idx] = 0.0f;

            r = local_id / TILE_N;
            c_idx = local_id % TILE_N;
            global_row = k_offset + r;
            global_col = blockIdx.x * TILE_N + c_idx;
            if (global_row < n && global_col < n)
                sb[r][c_idx] = b[global_row * n + global_col];
            else
                sb[r][c_idx] = 0.0f;
        }

        __syncthreads();

        // Compute sub-tile
        #pragma unroll
        for (int k = 0; k < TILE_K; ++k) {
            float a_vals[THREAD_M];
            float b_vals[THREAD_N];
            #pragma unroll
            for (int i = 0; i < THREAD_M; ++i) a_vals[i] = sa[ty * THREAD_M + i][k];
            #pragma unroll
            for (int j = 0; j < THREAD_N; ++j) b_vals[j] = sb[k][tx * THREAD_N + j];

            #pragma unroll
            for (int i = 0; i < THREAD_M; ++i) {
                #pragma unroll
                for (int j = 0; j < THREAD_N; ++j) {
                    res[i][j] += a_vals[i] * b_vals[j];
                }
            }
        }
        __syncthreads();
    }

    // Store results
    #pragma unroll
    for (int i = 0; i < THREAD_M; ++i) {
        for (int j = 0; j < THREAD_N; ++j) {
            int r = row_start + i;
            int col = col_start + j;
            if (r < n && col < n) {
                c[r * n + col] = res[i][j];
            }
        }
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const int TILE_N = 32;
    const int THREAD_M = 4;
    const int THREAD_N = 4;
    
    dim3 block(TILE_N / THREAD_N, TILE_N / THREAD_M);
    dim3 grid((n + TILE_N - 1) / TILE_N, (n + TILE_N - 1) / TILE_N);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<32, 32, 4, 4><<<grid, block>>>(a, b, c, n);
    }
}