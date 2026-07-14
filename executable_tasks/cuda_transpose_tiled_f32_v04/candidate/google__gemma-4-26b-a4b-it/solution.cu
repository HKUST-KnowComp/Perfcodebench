#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockDim = 16;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Use a padded shared memory array to avoid bank conflicts
    // Padding from 32 to 33 ensures that elements in a column are in different banks
    __shared__ float tile[kTileDim][kTileDim + 1];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load data into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new coordinates for the transposed output
    // The output index is (x, y) in the transposed space, which corresponds to (y, x) in original
    // However, we are mapping the tile. The thread (tx, ty) in the block will write to 
    // the transposed position.
    
    // New coordinates in the output matrix
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // We want to write to output[out_y * height + out_x] 
    // but we need to read from the tile in a way that is coalesced.
    // To achieve coalesced writes to output, threads in a warp must have contiguous out_x.
    // Since out_x = blockIdx.y * kTileDim + threadIdx.x, threadIdx.x provides contiguity.
    
    if (out_x < height && out_y < width) {
        // Read from shared memory using transposed indices to match the output's coalesced write
        // tile[threadIdx.x][threadIdx.y] is the transposed access
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more robust version using 32x32 threads to fully utilize the tile
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int col = blockIdx.x * kTileDim + threadIdx.x;
    int row = blockIdx.y * kTileDim + threadIdx.y;

    if (col < width && row < height) {
        tile[threadIdx.y][threadIdx.x] = input[row * width + col];
    }

    __syncthreads();

    // Transpose the indices for the output
    // The output width is the input height
    int out_col = blockIdx.y * kTileDim + threadIdx.x;
    int out_row = blockIdx.x * kTileDim + threadIdx.y;

    if (out_col < height && out_row < width) {
        output[out_row * height + out_col] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Using 32x32 threads to match the tile dimension for maximum efficiency
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}