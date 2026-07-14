#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions for shared memory
constexpr int TILE_DIM = 32;
// Block dimensions (threads per block)
constexpr int BLOCK_DIM = 16;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int bx = blockIdx.x;
    int by = blockIdx.y;

    // Each thread handles 2 elements to fill the 32x32 tile with a 16x16 block
    for (int i = 0; i < 2; ++i) {
        int x = bx * TILE_DIM + threadIdx.x;
        int y = by * TILE_DIM + threadIdx.y + (i * BLOCK_DIM);

        if (x < width && y < height) {
            tile[threadIdx.y + (i * BLOCK_DIM)][threadIdx.x] = input[y * width + x];
        } else {
            tile[threadIdx.y + (i * BLOCK_DIM)][threadIdx.x] = 0.0f;
        }
    }

    __syncthreads();

    // Transpose the tile and write to global memory
    // We need to map the 16x16 threads to the 32x32 tile for the output
    for (int i = 0; i < 2; ++i) {
        int x_out = bx * TILE_DIM + threadIdx.y + (i * BLOCK_DIM);
        int y_out = by * TILE_DIM + threadIdx.x;

        if (x_out < width && y_out < height) {
            // Coalesced write: output is indexed by [x_out * height + y_out]
            output[x_out * height + y_out] = tile[threadIdx.x][threadIdx.y + (i * BLOCK_DIM)];
        }
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