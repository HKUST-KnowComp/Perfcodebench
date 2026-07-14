#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int TILE_DIM = 32;
constexpr int TILE_PAD = 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + TILE_PAD];

    int col = blockIdx.x * TILE_DIM + threadIdx.x;
    int row = blockIdx.y * TILE_DIM + threadIdx.y;

    if (col < width && row < height) {
        tile[threadIdx.y][threadIdx.x] = input[row * width + col];
    }
    __syncthreads();

    int row_out = blockIdx.x * TILE_DIM + threadIdx.x;
    int col_out = blockIdx.y * TILE_DIM + threadIdx.y;

    if (row_out < height && col_out < width) {
        output[row_out * height + col_out] = tile[threadIdx.y][threadIdx.x];
    }
}
}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
