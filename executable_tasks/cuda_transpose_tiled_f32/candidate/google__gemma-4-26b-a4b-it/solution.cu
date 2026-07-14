#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Use shared memory with padding to avoid bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Read from input into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new coordinates for transposed output
    // The output index is (x, y) in the transposed space, which corresponds to (y, x) in original
    // However, we want to write to output such that output[y_new * width_new + x_new] is filled.
    // In transpose: output[col][row] = input[row][col].
    // New x (column in output) is the old y.
    // New y (row in output) is the old x.
    
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output
    // We swap threadIdx.x and threadIdx.y to ensure coalesced writes to the output matrix
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more efficient version using a 32x32 thread block to map to 32x32 shared memory
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int col = blockIdx.x * kTileDim + threadIdx.x;
    int row = blockIdx.y * kTileDim + threadIdx.y;

    if (col < width && row < height) {
        tile[threadIdx.y][threadIdx.x] = input[row * width + col];
    }

    __syncthreads();

    // Transpose indices: the threadIdx.x/y mapping is flipped for the write
    int new_row = blockIdx.x * kTileDim + threadIdx.x;
    int new_col = blockIdx.y * kTileDim + threadIdx.y;

    if (new_row < height && new_col < width) {
        output[new_col * height + new_row] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to maximize occupancy and use shared memory effectively
    dim3 block(kTileDim, kTileDim);
    // Grid covers the input dimensions
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}
