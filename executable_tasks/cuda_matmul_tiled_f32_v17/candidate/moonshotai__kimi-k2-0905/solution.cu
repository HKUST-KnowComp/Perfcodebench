#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 64;
constexpr int BLOCK = 16;
constexpr int UNROLL = 4;

__global__ void gemm_tiled_kernel(const float* __restrict__ A,
                                  const float* __restrict__ B,
                                  float* __restrict__ C,
                                  int n)
{
    const int tile_x = blockIdx.x;
    const int row_base = (threadIdx.y << 2) + (threadIdx.x >> 2) * 4;
    const int col_base = (threadIdx.x & 3) << 2;

    const int global_row = tile_x * TILE + row_base;
    const int global_col = tile_x * TILE + col_base;

    __shared__ float smem_A[TILE][TILE + 1];
    __shared__ float smem_B[TILE][TILE + 1];

    float4 fragC[4] = {};

    for (int k_tile = 0; k_tile < n; k_tile += TILE) {
        #pragma unroll
        for (int i = 0; i < 4; ++i) {
            int r = row_base + i;
            int c = col_base;
            if (global_row + i < n && c < n && k_tile + c < n) {
                float4 a = *reinterpret_cast<const float4*>(&A[(global_row + i) * n + k_tile + c]);
                smem_A[r][c + 0] = a.x;
                smem_A[r][c + 1] = a.y;
                smem_A[r][c + 2] = a.z;
                smem_A[r][c + 3] = a.w;
            }
        }

        #pragma unroll
        for (int i = 0; i < 4; ++i) {
            int r = row_base + i;
            int c = col_base;
            if (k_tile + r < n && global_col + i < n && global_col + i < n) {
                float4 b = *reinterpret_cast<const float4*>(&B[(k_tile + r) * n + global_col + i]);
                smem_B[r][c + 0] = b.x;
                smem_B[r][c + 1] = b.y;
                smem_B[r][c + 2] = b.z;
                smem_B[r][c + 3] = b.w;
            }
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE; k += UNROLL) {
            float4 a[UNROLL];
            float4 b[UNROLL];
            #pragma unroll
            for (int u = 0; u < UNROLL; ++u) {
                int kk = k + u;
                a[u] = make_float4(smem_A[row_base + 0][kk],
                                   smem_A[row_base + 1][kk],
                                   smem_A[row_base + 2][kk],
                                   smem_A[row_base + 3][kk]);
                b[u] = make_float4(smem_B[kk][col_base + 0],
                                   smem_B[kk][col_base + 1],
                                   smem_B[kk][col_base + 2],
                                   smem_B[kk][col_base + 3]);
            }
            #pragma unroll
            for (int u = 0; u < UNROLL; ++u) {
                fragC[0].x += a[u].x * b[u].x;
                fragC[0].y += a[u].x * b[u].y;
                fragC[0].z += a[u].x * b[u].z;
                fragC[0].w += a[u].x * b[u].w;

                fragC[1].x += a[u].y * b[u].x;
                fragC[1].y += a[u].y * b[u].y;
                fragC[1].z += a[u].y * b[u].z;
                fragC[1].w += a[u].y * b[u].w;

                fragC[2].x += a[u].z * b[u].x;
                fragC[2].y += a[u].z * b[u].y;
                fragC[2].z += a[u].z * b[u].z;
                fragC[2].w += a[u].z * b[u].w;

                fragC[3].x += a[u].w * b[u].x;
                fragC[3].y += a[u].w * b[u].y;
                fragC[3].z += a[u].w * b[u].z;
                fragC[3].w += a[u].w * b[u].w;
            }
        }
        __syncthreads();
    }

    #pragma unroll
    for (int i = 0; i < 4; ++i) {
        int r = global_row + i;
        int c = global_col;
        if (r < n && c < n) {
            *reinterpret_cast<float4*>(&C[r * n + c]) = fragC[i];
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const int tiles = (n + TILE - 1) / TILE;
    dim3 block(BLOCK, BLOCK);
    for (int i = 0; i < iters; ++i) {
        gemm_tiled_kernel<<<tiles, block>>>(a, b, c, n);
    }
}