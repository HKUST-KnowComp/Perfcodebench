#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;
constexpr int BLOCK_ROWS = 8;

__global__ void transpose_kernel(const float* __restrict__ input,
                                 float* __restrict__ output,
                                 int width, int height)
{
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

#pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
        }
    }

    __syncthreads();

    x = blockIdx.y * TILE_DIM + threadIdx.x;
    y = blockIdx.x * TILE_DIM + threadIdx.y;

#pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < height && (y + j) < width) {
            output[(y + j) * height + x] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, BLOCK_ROWS);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM,
              (height + TILE_DIM - 1) / TILE_DIM);
    for (int i = 0; i < iters; ++i) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}