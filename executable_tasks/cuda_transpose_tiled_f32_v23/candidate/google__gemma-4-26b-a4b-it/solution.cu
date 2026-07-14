#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding to avoid shared memory bank conflicts
constexpr int kTileDimPadded = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to prevent bank conflicts
    __shared__ float tile[kTileDimPadded][kTileDimPadded];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load from input into shared memory
    // We use threadIdx.x and threadIdx.y to map to the tile
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new coordinates for the transposed output
    // The output index is (x, y) in the transposed space
    // which corresponds to (y, x) in the original space.
    // However, we want to write to output[y_new * width_new + x_new]
    // where width_new = height and height_new = width.
    
    // Re-map thread indices for the output write
    // To ensure coalesced writes, we want threadIdx.x to map to the contiguous dimension of the output.
    // The output matrix has width = height and height = width.
    // Output index: [new_y * height + new_x]
    // We want new_x to be the fast-moving index (threadIdx.x)
    
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    if (out_x < height && out_y < width) {
        // Read from shared memory using transposed indices
        // tile[threadIdx.y][threadIdx.x] was loaded from input[y][x]
        // We want to write tile[threadIdx.x][threadIdx.y] to output[out_y][out_x]
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to match the tile dimension
    // The grid is calculated based on the input dimensions
    // Note: The output dimensions are (height x width)
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
