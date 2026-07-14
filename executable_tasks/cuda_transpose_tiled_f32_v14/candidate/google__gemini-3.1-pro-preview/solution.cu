#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;
constexpr int BLOCK_ROWS = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    // Calculate global coordinates for the input matrix
    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Read from input into shared memory (coalesced)
    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
        }
    }

    __syncthreads();

    // Calculate global coordinates for the output matrix (transposed block indices)
    // New x is the old y-block, new y is the old x-block
    int out_x = blockIdx.y * TILE_DIM + threadIdx.x;
    int out_y = blockIdx.x * TILE_DIM + threadIdx.y;

    // Write from shared memory to output (coalesced)
    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (out_x < height && (out_y + j) < width) {
            output[(out_y + j) * height + out_x] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    if (width <= 0 || height <= 0) return;

    const dim3 block(TILE_DIM, BLOCK_ROWS);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}