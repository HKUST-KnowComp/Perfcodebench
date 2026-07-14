#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
    __shared__ float As[TILE_DIM][TILE_DIM];
    __shared__ float Bs[TILE_DIM][TILE_DIM];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row = blockIdx.y * TILE_DIM + ty;
    const int col = blockIdx.x * TILE_DIM + tx;

    double sum = 0.0;

    const int num_tiles = (n + TILE_DIM - 1) / TILE_DIM;

    for (int t = 0; t < num_tiles; ++t) {
        if (row < n && (t * TILE_DIM + tx) < n) {
            As[ty][tx] = a[row * n + (t * TILE_DIM + tx)];
        } else {
            As[ty][tx] = 0.0f;
        }

        if ((t * TILE_DIM + ty) < n && col < n) {
            Bs[ty][tx] = b[(t * TILE_DIM + ty) * n + col];
        } else {
            Bs[ty][tx] = 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_DIM; ++k) {
            sum += static_cast<double>(As[ty][k]) * static_cast<double>(Bs[k][tx]);
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = static_cast<float>(sum);
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}