#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }

    __syncthreads();

    x = blockIdx.y * TILE_DIM + threadIdx.x;
    y = blockIdx.x * TILE_DIM + threadIdx.y;

    if (x < height && y < width) {
        output[x * height + y] = tile[threadIdx.x][threadIdx.y];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}