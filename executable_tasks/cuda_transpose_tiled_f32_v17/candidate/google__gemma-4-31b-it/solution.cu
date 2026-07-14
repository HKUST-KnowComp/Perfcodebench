#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Tile dimensions for shared memory
constexpr int TILE_DIM = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile to store a block of the input matrix
    // Adding +1 to the second dimension helps avoid shared memory bank conflicts
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Load data from global memory into shared memory
    // Each thread in the 32x8 block loads 4 elements to fill the 32x32 tile
    for (int i = 0; i < TILE_DIM; i += 8) {
        int curr_y = y + i;
        if (x < width && curr_y < height) {
            tile[threadIdx.y + i][threadIdx.x] = input[curr_y * width + x];
        }
    }

    // Synchronize to ensure the entire tile is loaded
    __syncthreads();

    // Calculate the output coordinates
    // The output is the transpose, so we swap x and y indices
    // We want to write to output in a coalesced manner
    int out_x = blockIdx.y * TILE_DIM + threadIdx.x;
    int out_y = blockIdx.x * TILE_DIM + threadIdx.y;

    // Each thread in the 32x8 block writes 4 elements to the output
    for (int i = 0; i < TILE_DIM; i += 8) {
        int curr_out_y = out_y + i;
        if (out_x < height && curr_out_y < width) {
            // Read from shared memory (transposed) and write to global memory
            output[curr_out_y * height + out_x] = tile[threadIdx.x][threadIdx.y + i];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Block size: 32 threads in x, 8 threads in y
    const dim3 block(TILE_DIM, 8);
    // Grid size: cover the entire matrix with 32x32 tiles
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}