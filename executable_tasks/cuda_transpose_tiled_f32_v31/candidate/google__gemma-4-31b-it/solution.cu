#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile size chosen for balance between occupancy and memory coalescing
constexpr int TILE_DIM = 32;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Use shared memory to buffer a tile. 
    // Padding the second dimension to 33 prevents bank conflicts when accessing columns.
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Load from global memory into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new coordinates for the transposed write
    // The block is now reading from the shared memory tile in a transposed manner
    // and writing to global memory in a coalesced manner.
    int x_out = blockIdx.y * TILE_DIM + threadIdx.x;
    int y_out = blockIdx.x * TILE_DIM + threadIdx.y;

    if (x_out < width && y_out < height) {
        output[y_out * width + x_out] = tile[threadIdx.x][threadIdx.y];
    } else if (x_out < width && y_out < height) {
        // This part is technically handled by the if condition, 
        // but we ensure we only write within bounds of the output matrix.
    }
    
    // Note: The output index is calculated as [y_out * width + x_out] 
    // because the output matrix is also width x height (transposed dimensions).
    // Wait, the original code used output[x * height + (y + j)]. 
    // Let's correct the indexing to match the original logic: 
    // Input: (height, width) -> Output: (width, height)
}

// Corrected kernel to match the specific indexing of the original task
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Transpose indices for the output
    // Original: output[x * height + y]
    int x_out = blockIdx.y * TILE_DIM + threadIdx.x;
    int y_out = blockIdx.x * TILE_DIM + threadIdx.y;

    if (x_out < height && y_out < width) {
        // We want to write the value from tile[threadIdx.x][threadIdx.y]
        // which corresponds to input[y_out * width + x_out]
        // to output[x_out * height + y_out]
        // Wait, the logic is: input[y][x] -> output[x][y]
        // In the tile, we stored input[y][x] at tile[ty][tx]
        // We want to write tile[ty][tx] to output[tx * height + ty]
        // To make it coalesced, we need the inner-most index to be the threadIdx.x
        // Let's use the standard tiled transpose pattern:
        // Read: input[y][x] -> tile[ty][tx]
        // Write: tile[tx][ty] -> output[x_out][y_out]
    }
}

// Refined kernel for maximum performance and correctness
__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int col = blockIdx.x * TILE_DIM + tx;
    int row = blockIdx.y * TILE_DIM + ty;

    if (col < width && row < height) {
        tile[ty][tx] = input[row * width + col];
    } else {
        tile[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Transpose the tile: read from tile[tx][ty], write to output
    // New coordinates: row_out = blockIdx.x * TILE_DIM + ty, col_out = blockIdx.y * TILE_DIM + tx
    int row_out = blockIdx.x * TILE_DIM + ty;
    int col_out = blockIdx.y * TILE_DIM + tx;

    if (row_out < width && col_out < height) {
        output[row_out * height + col_out] = tile[tx][ty];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
