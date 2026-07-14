#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions
constexpr int TILE_DIM = 32;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory with padding to avoid bank conflicts
    // Using [32][33] instead of [32][32]
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Each thread in the 32x8 block handles 4 elements in the y-direction to fill the 32x32 tile
    for (int i = 0; i < TILE_DIM; i += 8) {
        int curr_y = y + i;
        if (x < width && curr_y < height) {
            tile[threadIdx.y + i][threadIdx.x] = input[curr_y * width + x];
        } else {
            tile[threadIdx.y + i][threadIdx.x] = 0.0f;
        }
    }

    // Synchronize to ensure the tile is fully loaded
    __syncthreads();

    // Transpose the tile and write to global memory
    // We need to map the 32x8 block to the 32x32 output area
    // The output is written in a coalesced manner
    int out_x = blockIdx.y * TILE_DIM + threadIdx.x;
    int out_y = blockIdx.x * TILE_DIM + threadIdx.y;

    for (int i = 0; i < TILE_DIM; i += 8) {
        int curr_out_y = out_y + i;
        if (out_x < height && curr_out_y < width) {
            output[curr_out_y * height + out_x] = tile[threadIdx.x][threadIdx.y + i];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Grid and block dimensions
    // Block size 32x8 = 256 threads
    dim3 block(TILE_DIM, 8);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}