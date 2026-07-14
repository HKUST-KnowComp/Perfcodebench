#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ A,
                                    const float* __restrict__ B,
                                    float* __restrict__ C,
                                    int N)
{
    __shared__ float As[TILE][TILE + 1];   // +1 to avoid bank conflicts
    __shared__ float Bs[TILE][TILE + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = blockIdx.y * TILE + ty;
    int col = blockIdx.x * TILE + tx;

    float sum = 0.0f;

    for (int ph = 0; ph < (N + TILE - 1) / TILE; ++ph) {
        int a_col = ph * TILE + tx;
        int b_row = ph * TILE + ty;

        As[ty][tx] = (row < N && a_col < N) ? A[row * N + a_col] : 0.0f;
        Bs[ty][tx] = (b_row < N && col < N) ? B[b_row * N + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE; ++k) {
            sum += As[ty][k] * Bs[k][tx];
        }
        __syncthreads();
    }

    if (row < N && col < N)
        C[row * N + col] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(TILE, TILE);
    dim3 grid((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
