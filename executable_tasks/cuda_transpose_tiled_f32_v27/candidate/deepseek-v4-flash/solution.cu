#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1]; // padded to avoid bank conflicts

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // coalesced read from input
    if (x < width && y < height)
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    else
        tile[threadIdx.y][threadIdx.x] = 0.0f;

    __syncthreads();

    // coalesced write to output (transposed order)
    int out_row = blockIdx.x * TILE_DIM + threadIdx.x;
    int out_col = blockIdx.y * TILE_DIM + threadIdx.y;
    if (out_row < width && out_col < height)
        output[out_row * height + out_col] = tile[threadIdx.y][threadIdx.x];
}

} // anonymous namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
