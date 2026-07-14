#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    __shared__ float As[32][32];
    __shared__ float Bs[32][32];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = by * 32 + ty;
    int col = bx * 32 + tx;

    float sum = 0.0f;

    int num_tiles = (n + 31) / 32;
    for (int t = 0; t < num_tiles; ++t) {
        int a_col = t * 32 + tx;
        int b_row = t * 32 + ty;

        As[ty][tx] = (row < n && a_col < n) ? a[row * n + a_col] : 0.0f;
        Bs[ty][tx] = (b_row < n && col < n) ? b[b_row * n + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < 32; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(32, 32);
    dim3 grid((n + 31) / 32, (n + 31) / 32);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}