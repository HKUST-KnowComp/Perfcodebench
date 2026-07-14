#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
    const int tile_size = 16;
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int i_start = blockIdx.y * tile_size;
    const int j_start = blockIdx.x * tile_size;
    const int i = i_start + ty;
    const int j = j_start + tx;

    if (i >= n || j >= n) {
        return;
    }

    __shared__ float shared_a[16][16];
    __shared__ float shared_b[16][16];

    float sum = 0.0f;

    for (int k = 0; k < n; k += tile_size) {
        if (k + tx < n) {
            shared_a[ty][tx] = a[i * n + (k + tx)];
        } else {
            shared_a[ty][tx] = 0.0f;
        }

        if (k + ty < n) {
            shared_b[ty][tx] = b[(k + ty) * n + j];
        } else {
            shared_b[ty][tx] = 0.0f;
        }

        __syncthreads();

        for (int l = 0; l < tile_size; ++l) {
            sum += shared_a[ty][l] * shared_b[l][tx];
        }

        __syncthreads();
    }

    c[i * n + j] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(16, 16);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}