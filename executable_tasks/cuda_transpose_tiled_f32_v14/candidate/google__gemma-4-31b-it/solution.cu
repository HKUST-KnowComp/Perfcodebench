#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions
constexpr int TILE_DIM = 32;
// Block dimensions (16x16 threads, each handling 2 elements to fill 32x32 tile)
constexpr int BLOCK_DIM = 16;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Each thread loads two elements to cover the 32x32 tile with 16x16 threads
    // Element 1
    int x0 = bx * TILE_DIM + tx;
    int y0 = by * TILE_DIM + ty;
    if (x0 < width && y0 < height) {
        tile[ty][tx] = input[y0 * width + x0];
    } else {
        tile[ty][tx] = 0.0f;
    }

    // Element 2 (offset by 16)
    int x1 = bx * TILE_DIM + tx + BLOCK_DIM;
    int y1 = by * TILE_DIM + ty + BLOCK_DIM;
    if (x1 < width && y1 < height) {
        tile[ty + BLOCK_DIM][tx] = input[y1 * width + x1];
    } else {
        tile[ty + BLOCK_DIM][tx] = 0.0f;
    }

    __syncthreads();

    // Calculate output coordinates
    // The output is transposed, so the block indices are swapped
    // We need to write coalesced, so we map threads to output columns
    int x_out = by * TILE_DIM + tx;
    int y_out = bx * TILE_DIM + ty;

    // Write Element 1
    if (x_out < height && y_out < width) {
        output[y_out * height + x_out] = tile[tx][ty];
    }

    // Write Element 2 (offset by 16)
    if (x_out < height && y_out + BLOCK_DIM < width) {
        output[(y_out + BLOCK_DIM) * height + x_out] = tile[tx + BLOCK_DIM][ty];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(BLOCK_DIM, BLOCK_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}