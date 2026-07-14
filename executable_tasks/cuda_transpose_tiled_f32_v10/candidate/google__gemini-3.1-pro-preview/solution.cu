#include "interface.h"
#include <cuda_runtime.h>

namespace {

// TILE_DIM 32 is optimal for coalescing and shared memory usage on most architectures.
// BLOCK_ROWS 8 allows each thread to process 4 elements per tile, improving instruction density.
constexpr int TILE_DIM = 32;
constexpr int BLOCK_ROWS = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts (32x33 instead of 32x32).
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    // Calculate global coordinates for the input matrix.
    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Read from input into shared memory.
    // Reads are coalesced because threadIdx.x is the fastest-moving index.
    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
        }
    }

    __syncthreads();

    // Calculate global coordinates for the output matrix.
    // We transpose the block indices and the thread indices to maintain coalesced writes.
    int x_out = blockIdx.y * TILE_DIM + threadIdx.x;
    int y_out = blockIdx.x * TILE_DIM + threadIdx.y;

    // Write from shared memory to output.
    // Writes are coalesced because threadIdx.x is the fastest-moving index for the output row.
    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x_out < height && (y_out + j) < width) {
            output[(y_out + j) * height + x_out] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    if (width <= 0 || height <= 0) return;

    const dim3 block(TILE_DIM, BLOCK_ROWS);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}