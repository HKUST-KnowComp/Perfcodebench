#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Use 32x32 tile to maximize occupancy and shared memory usage.
// Padding by 1 (32x33) avoids bank conflicts during shared memory access.
constexpr int TILE_DIM = 32;
constexpr int BLOCK_ROWS = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
        }
    }

    __syncthreads();

    int x_out = blockIdx.y * TILE_DIM + threadIdx.x;
    int y_out = blockIdx.x * TILE_DIM + threadIdx.y;

    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x_out < height && (y_out + j) < width) {
            output[(y_out + j) * height + x_out] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(TILE_DIM, BLOCK_ROWS);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}