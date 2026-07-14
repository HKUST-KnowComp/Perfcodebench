#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions
const int BM = 128;
const int BN = 128;
const int BK = 8;
const int TM = 8;
const int TN = 8;

__global__ void matmul_optimized_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Shared memory for tiles
    __shared__ float s_a[BM][BK];
    __shared__ float s_b[BK][BN];

    // Register fragments for C
    float r_c[TM][TN] = {0.0f};
    float r_a[TM];
    float r_b[TN];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Thread ID within the 16x16 block
    int tid = ty * (BN / TN) + tx;

    // Loading logic for shared memory using float4
    // Each thread loads one float4 from A and one from B
    // A tile: 128x8, B tile: 8x128
    int load_a_row = tid / 2; // 0..127
    int load_a_col = (tid % 2) * 4; // 0 or 4
    int load_b_row = tid / 32; // 0..7
    int load_b_col = (tid % 32) * 4; // 0, 4, ..., 124

    for (int k_tile = 0; k_tile < n; k_tile += BK) {
        // Load A into shared memory
        if (by * BM + load_a_row < n && k_tile + load_a_col < n) {
            float4 tmp_a = *reinterpret_cast<const float4*>(&a[(by * BM + load_a_row) * n + (k_tile + load_a_col)]);
            s_a[load_a_row][load_a_col + 0] = tmp_a.x;
            s_a[load_a_row][load_a_col + 1] = tmp_a.y;
            s_a[load_a_row][load_a_col + 2] = tmp_a.z;
            s_a[load_a_row][load_a_col + 3] = tmp_a.w;
        } else {
            s_a[load_a_row][load_a_col + 0] = 0.0f;
            s_a[load_a_row][load_a_col + 1] = 0.0f;
            s_a[load_a_row][load_a_col + 2] = 0.0f;
            s_a[load_a_row][load_a_col + 3] = 0.0f;
        }

        // Load B into shared memory
        if (k_tile + load_b_row < n && bx * BN + load_b_col < n) {
            *reinterpret_cast<float4*>(&s_b[load_b_row][load_b_col]) = *reinterpret_cast<const float4*>(&b[(k_tile + load_b_row) * n + (bx * BN + load_b_col)]);
        } else {
            *reinterpret_cast<float4*>(&s_b[load_b_row][load_b_col]) = make_float4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        __syncthreads();

        // Compute outer product
        #pragma unroll
        for (int k = 0; k < BK; ++k) {
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                r_a[i] = s_a[ty * TM + i][k];
            }
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                r_b[j] = s_b[k][tx * TN + j];
            }
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                #pragma unroll
                for (int j = 0; j < TN; ++j) {
                    r_c[i][j] += r_a[i] * r_b[j];
                }
            }
        }
        __syncthreads();
    }

    // Write results back to global memory
    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        int row = by * BM + ty * TM + i;
        if (row < n) {
            #pragma unroll
            for (int j = 0; j < TN; j += 4) {
                int col = bx * BN + tx * TN + j;
                if (col < n) {
                    float4 res;
                    res.x = r_c[i][j + 0];
                    res.y = r_c[i][j + 1];
                    res.z = r_c[i][j + 2];
                    res.w = r_c[i][j + 3];
                    *reinterpret_cast<float4*>(&c[row * n + col]) = res;
                }
            }
        }
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    // Block size: 16x16 threads, each computing 8x8 elements
    dim3 block(BN / TN, BM / TM);
    dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_optimized_kernel<<<grid, block>>>(a, b, c, n);
    }
}