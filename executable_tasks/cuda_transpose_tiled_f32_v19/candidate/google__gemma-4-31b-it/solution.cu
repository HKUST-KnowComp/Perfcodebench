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

    // Each thread handles 2 elements to fill the 32x32 tile with 16x16 threads
    for (int i = 0; i < 2; ++i) {
        int x = bx * TILE_DIM + threadIdx.x + (i * BLOCK_DIM);
        int y = by * TILE_DIM + threadIdx.y + (i * BLOCK_DIM);

        if (x < width && y < height) {
            tile[threadIdx.y + (i * BLOCK_DIM)][threadIdx.x + (i * BLOCK_DIM)] = input[y * width + x];
        } else {
            tile[threadIdx.y + (i * BLOCK_DIM)][threadIdx.x + (i * BLOCK_DIM)] = 0.0f;
        }
    }

    __syncthreads();

    // Calculate output coordinates
    // The output is transposed, so the block indices are swapped
    // We need to map the shared memory tile to the output matrix
    int x_out = bx * TILE_DIM + threadIdx.x;
    int y_out = by * TILE_DIM + threadIdx.y;

    for (int i = 0; i < 2; ++i) {
        // Read from shared memory in a transposed manner
        // threadIdx.x becomes the row index, threadIdx.y becomes the col index
        int sh_row = threadIdx.x + (i * BLOCK_DIM);
        int sh_col = threadIdx.y + (i * BLOCK_DIM);

        // The output index is (col_in * height + row_in)
        // Here, the original 'x' (column) becomes the new row, and 'y' (row) becomes the new column
        int out_row = bx * TILE_DIM + sh_row;
        int out_col = by * TILE_DIM + sh_col;

        if (out_row < width && out_col < height) {
            output[out_row * height + out_col] = tile[sh_col][sh_row];
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