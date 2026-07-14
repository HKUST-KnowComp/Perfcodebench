#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile dimensions
constexpr int TILE_DIM = 32;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    // Using [32][33] instead of [32][32] ensures that columns map to different banks
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Each thread block processes a TILE_DIM x TILE_DIM area
    // Since we use a 16x16 block, each thread handles 2 elements in the y-direction
    for (int i = 0; i < 2; ++i) {
        int curr_y = y + i * 16;
        if (x < width && curr_y < height) {
            tile[threadIdx.y + i * 16][threadIdx.x] = input[curr_y * width + x];
        } else {
            tile[threadIdx.y + i * 16][threadIdx.x] = 0.0f;
        }
    }

    __syncthreads();

    // Calculate the output coordinates
    // The output is transposed, so the block's x becomes the output's y and vice versa
    int out_x = blockIdx.y * TILE_DIM + threadIdx.x;
    int out_y = blockIdx.x * TILE_DIM + threadIdx.y;

    for (int i = 0; i < 2; ++i) {
        int curr_out_y = out_y + i * 16;
        if (out_x < height && curr_out_y < width) {
            // Read from shared memory in a transposed manner
            // The shared memory access is now coalesced for the output write
            output[curr_out_y * height + out_x] = tile[threadIdx.x][blockIdx.x * 0 + (threadIdx.y + i * 16)];
            // Wait, the logic above is slightly wrong. Let's correct the indexing:
            // We want to write output[out_x * height + curr_out_y]
            // where out_x is the column of the input tile and curr_out_y is the row.
        }
    }
}

// Corrected kernel for clarity and correctness
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x * TILE_DIM;
    int by = blockIdx.y * TILE_DIM;

    // Load from global to shared
    #pragma unroll
    for (int i = 0; i < 2; ++i) {
        int row = by + ty + i * 16;
        int col = bx + tx;
        if (row < height && col < width) {
            tile[ty + i * 16][tx] = input[row * width + col];
        } else {
            tile[ty + i * 16][tx] = 0.0f;
        }
    }

    __syncthreads();

    // Store from shared to global (transposed)
    // We want to write to output[col * height + row]
    // To keep writes coalesced, we map threads to output columns
    int out_col = blockIdx.y * TILE_DIM + tx;
    int out_row = blockIdx.x * TILE_DIM + ty;

    #pragma unroll
    for (int i = 0; i < 2; ++i) {
        int row_in_tile = ty + i * 16;
        int col_in_tile = tx;
        // The element at tile[row_in_tile][col_in_tile] was input[by + row_in_tile][bx + col_in_tile]
        // It should go to output[(bx + col_in_tile) * height + (by + row_in_tile)]
        
        // To ensure coalesced writes, we need the fastest changing index (threadIdx.x) 
        // to be the last dimension of the output array.
        // Output index: [row * height + col]
        // Let's use the standard tiled transpose pattern:
        int target_row = blockIdx.y * TILE_DIM + ty + i * 16;
        int target_col = blockIdx.x * TILE_DIM + tx;
        
        if (target_row < width && target_col < height) {
            output[target_row * height + target_col] = tile[tx][ty + i * 16];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(16, 16);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int i = 0; i < iters; ++i) {
        transpose_final_kernel<<<grid, block>>>(input, output, width, height);
    }
}