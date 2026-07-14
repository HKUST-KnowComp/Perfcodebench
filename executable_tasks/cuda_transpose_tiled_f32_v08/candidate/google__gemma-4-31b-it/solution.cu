#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions for shared memory
constexpr int TILE_DIM = 32;
// Block dimensions (threads per block)
constexpr int BLOCK_DIM = 16;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory with padding to avoid bank conflicts
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int bx = blockIdx.x;
    int by = blockIdx.y;

    // Each thread handles 2 elements to fill the 32x32 tile with 16x16 threads
    for (int i = 0; i < 2; ++i) {
        int x = bx * TILE_DIM + threadIdx.x + (i * BLOCK_DIM);
        int y = by * TILE_DIM + threadIdx.y + (i * BLOCK_DIM);

        if (x < width && y < height) {
            tile[threadIdx.y + (i * BLOCK_DIM)][threadIdx.x] = input[y * width + x];
        } else {
            tile[threadIdx.y + (i * BLOCK_DIM)][threadIdx.x] = 0.0f;
        }
    }

    __syncthreads();

    // Transpose the tile and write to global memory coalesced
    // We want to write to output[x * height + y]
    // To make writes coalesced, threads in a warp must write to contiguous addresses
    // output index: (col * height) + row
    // We map threadIdx.x to the row of the output (which is the col of the input)
    // and threadIdx.y to the col of the output (which is the row of the input)
    
    for (int i = 0; i < 2; ++i) {
        int x_out = bx * TILE_DIM + threadIdx.y + (i * BLOCK_DIM);
        int y_out = by * TILE_DIM + threadIdx.x + (i * BLOCK_DIM);

        if (x_out < width && y_out < height) {
            // Read from shared memory (transposed) and write to global memory
            // The shared memory index is [row][col]. We want [col][row].
            output[x_out * height + y_out] = tile[threadIdx.x + (i * BLOCK_DIM)][threadIdx.y];
        }
    }
    
    // Note: The logic above for the second loop needs to be carefully aligned for coalescing.
    // Let's refine the write phase for better coalescing:
}

// Refined kernel for better coalescing
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Load tile from global to shared
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }

    __syncthreads();

    // Calculate output coordinates
    // We want to write to output[x_out * height + y_out]
    // To be coalesced, threadIdx.x should map to y_out
    int x_out = blockIdx.y * TILE_DIM + threadIdx.x;
    int y_out = blockIdx.x * TILE_DIM + threadIdx.y;

    if (x_out < height && y_out < width) {
        output[y_out * height + x_out] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Use 32x32 blocks for simplicity and efficiency
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_final_kernel<<<grid, block>>>(input, output, width, height);
    }
}
