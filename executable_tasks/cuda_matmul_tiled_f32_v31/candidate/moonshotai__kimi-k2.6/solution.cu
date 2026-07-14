#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TS = 32;

__launch_bounds__(TS * TS, 2)
__global__ void matmul_tiled_kernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c,
                                    int n) {
    __shared__ float sA[TS][TS + 1];
    __shared__ float sB[TS][TS + 1];

    const int bx = blockIdx.x;
    const int by = blockIdx.y;
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;

    const int row = by * TS + ty;
    const int col = bx * TS + tx;

    const int rowOffset = row * n;
    float sum = 0.0f;
    const int numTiles = (n + TS - 1) / TS;

    for (int t = 0; t < numTiles; ++t) {
        const int tileOffset = t * TS;

        const int aCol = tileOffset + tx;
        sA[ty][tx] = (row < n && aCol < n) ? a[rowOffset + aCol] : 0.0f;

        const int bRow = tileOffset + ty;
        const int bRowOffset = bRow * n;
        sB[ty][tx] = (bRow < n && col < n) ? b[bRowOffset + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TS; ++k) {
            sum += sA[ty][k] * sB[k][tx];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[rowOffset + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(TS, TS);
    const dim3 grid((n + TS - 1) / TS, (n + TS - 1) / TS);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
