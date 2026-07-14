#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile size for shared memory
constexpr int TILE_DIM = 32;
// Block size for threads
constexpr int BLOCK_DIM = 16;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    // [TILE_DIM][TILE_DIM + 1] ensures that columns map to different banks
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Each thread in the 16x16 block processes 4 elements to cover the 32x32 tile
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            int curr_x = x + j * BLOCK_DIM;
            int curr_y = y + i * BLOCK_DIM;

            if (curr_x < width && curr_y < height) {
                tile[i * BLOCK_DIM + threadIdx.y][j * BLOCK_DIM + threadIdx.x] = input[curr_y * width + curr_x];
            } else {
                tile[i * BLOCK_DIM + threadIdx.y][j * BLOCK_DIM + threadIdx.x] = 0.0f;
            }
        }
    }

    __syncthreads();

    // Calculate the output coordinates
    // The block is transposed: blockIdx.x becomes the y-offset, blockIdx.y becomes the x-offset
    int out_x = blockIdx.y * TILE_DIM + threadIdx.x;
    int out_y = blockIdx.x * TILE_DIM + threadIdx.y;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            int curr_out_x = out_x + j * BLOCK_DIM;
            int curr_out_y = out_y + i * BLOCK_DIM;

            if (curr_out_x < width && curr_out_y < height) {
                // Read from shared memory in a transposed manner
                output[curr_out_x * height + curr_out_y] = tile[j * BLOCK_DIM + threadIdx.x][i * BLOCK_DIM + threadIdx.y];
            }
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(BLOCK_DIM, BLOCK_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}