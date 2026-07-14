#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions
constexpr int TILE_DIM = 32;
// Thread block dimensions (16x16 threads handle a 32x32 tile)
constexpr int BLOCK_DIM = 16;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Each thread loads two elements to fill the 32x32 tile
    // First element
    int x0 = bx * TILE_DIM + tx;
    int y0 = by * TILE_DIM + ty;
    if (x0 < width && y0 < height) {
        tile[ty][tx] = input[y0 * width + x0];
    } else {
        tile[ty][tx] = 0.0f;
    }

    // Second element (offset by 16 in both dimensions)
    int x1 = bx * TILE_DIM + tx + BLOCK_DIM;
    int y1 = by * TILE_DIM + ty + BLOCK_DIM;
    if (x1 < width && y1 < height) {
        tile[ty + BLOCK_DIM][tx + BLOCK_DIM] = input[y1 * width + x1];
    } else {
        tile[ty + BLOCK_DIM][tx + BLOCK_DIM] = 0.0f;
    }

    // Handle the remaining parts of the 32x32 tile for the 16x16 thread block
    // Third element
    int x2 = bx * TILE_DIM + tx + BLOCK_DIM;
    int y2 = by * TILE_DIM + ty;
    if (x2 < width && y2 < height) {
        tile[ty][tx + BLOCK_DIM] = input[y2 * width + x2];
    } else {
        tile[ty][tx + BLOCK_DIM] = 0.0f;
    }

    // Fourth element
    int x3 = bx * TILE_DIM + tx;
    int y3 = by * TILE_DIM + ty + BLOCK_DIM;
    if (x3 < width && y3 < height) {
        tile[ty + BLOCK_DIM][tx] = input[y3 * width + x3];
    } else {
        tile[ty + BLOCK_DIM][tx] = 0.0f;
    }

    __syncthreads();

    // Transpose the tile and write to global memory coalescedly
    // We want to write to output[x * height + y]
    // To make writes coalesced, threads in a warp should write to contiguous output indices
    // Output index: (bx * TILE_DIM + ty) * height + (by * TILE_DIM + tx)
    
    // Map threads to the transposed tile
    // Thread (tx, ty) writes to output based on the transposed coordinates
    // We need to cover the 32x32 tile using the 16x16 block
    
    // Part 1
    int out_x0 = bx * TILE_DIM + ty;
    int out_y0 = by * TILE_DIM + tx;
    if (out_x0 < width && out_y0 < height) {
        output[out_x0 * height + out_y0] = tile[tx][ty];
    }

    // Part 2
    int out_x1 = bx * TILE_DIM + ty + BLOCK_DIM;
    int out_y1 = by * TILE_DIM + tx;
    if (out_x1 < width && out_y1 < height) {
        output[out_x1 * height + out_y1] = tile[tx + BLOCK_DIM][ty];
    }

    // Part 3
    int out_x2 = bx * TILE_DIM + ty;
    int out_y2 = by * TILE_DIM + tx + BLOCK_DIM;
    if (out_x2 < width && out_y2 < height) {
        output[out_x2 * height + out_y2] = tile[tx][ty + BLOCK_DIM];
    }

    // Part 4
    int out_x3 = bx * TILE_DIM + ty + BLOCK_DIM;
    int out_y3 = by * TILE_DIM + tx + BLOCK_DIM;
    if (out_x3 < width && out_y3 < height) {
        output[out_x3 * height + out_y3] = tile[tx + BLOCK_DIM][ty + BLOCK_DIM];
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