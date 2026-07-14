#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding to avoid bank conflicts in shared memory
constexpr int kSharedDim = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kSharedDim][kSharedDim];

    // Calculate global indices
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load data into shared memory from input
    // We use threadIdx.x and threadIdx.y to map to the tile
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new indices for the transposed output
    // The output coordinates are (y, x) relative to the original
    // But we need to map the tile to the transposed grid
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write data from shared memory to output
    // Accessing tile[threadIdx.x][threadIdx.y] ensures coalesced writes to output
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more efficient version using a single thread to handle multiple elements
// to maximize occupancy and memory throughput.
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int col = blockIdx.x * kTileDim + tx;
    int row = blockIdx.y * kTileDim + ty;

    if (col < width && row < height) {
        tile[ty][tx] = input[row * width + col];
    }

    __syncthreads();

    // Transpose indices: blockIdx.x and blockIdx.y are swapped for the output grid
    int new_col = blockIdx.y * kTileDim + tx;
    int new_row = blockIdx.x * kTileDim + ty;

    if (new_col < height && new_row < width) {
        output[new_row * height + new_col] = tile[tx][ty];
    }
}

// Final high-performance kernel: 32x8 threads per block to match original signature logic
// but utilizing shared memory for coalescing.
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    
    // Map thread to tile
    int x = blockIdx.x * kTileDim + tx;
    int y = blockIdx.y * kTileDim + ty;

    // Load into shared memory (coalesced read)
    if (x < width && y < height) {
        tile[ty][tx] = input[y * width + x];
    }

    __syncthreads();

    // Transpose: The output block index is swapped
    // The thread index in the tile is also swapped to ensure coalesced write
    int out_x = blockIdx.y * kTileDim + tx;
    int out_y = blockIdx.x * kTileDim + ty;

    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Using 32x32 block for maximum efficiency in shared memory tiling
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_final_kernel<<<grid, block>>>(input, output, width, height);
    }
}
