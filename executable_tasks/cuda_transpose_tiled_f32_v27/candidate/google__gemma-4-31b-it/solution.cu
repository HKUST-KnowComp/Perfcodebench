#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions for shared memory
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

    // Each thread loads two elements to fill the 32x32 tile using 16x16 threads
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

    // Calculate output coordinates for coalesced writes
    // The output is transposed, so we map the shared memory tile to the output matrix
    // We need to read from tile in a way that writes to output are contiguous
    int out_x = by * TILE_DIM + tx;
    int out_y = bx * TILE_DIM + ty;

    // Write Element 1
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[tx][ty];
    }

    // Write Element 2 (offset by 16)
    if (out_x < height && out_y + BLOCK_DIM < width) {
        output[(out_y + BLOCK_DIM) * height + out_x] = tile[tx + BLOCK_DIM][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(BLOCK_DIM, BLOCK_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int i = 0; i < iters; ++i) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}