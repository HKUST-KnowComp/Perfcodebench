#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding the shared memory dimension to avoid bank conflicts
constexpr int kSharedDim = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kSharedDim][kSharedDim];

    // Calculate global coordinates
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

    // Calculate new coordinates for the transposed output
    // The output x is the old y, and output y is the old x
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write data from shared memory to output
    // Note: We read from tile[threadIdx.x][threadIdx.y] to perform the transpose
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more robust version using a 32x8 block to match the original thread layout if needed,
// but a 32x32 block is generally more efficient for full tile coverage.
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
    int out_col = blockIdx.y * kTileDim + tx;
    int out_row = blockIdx.x * kTileDim + ty;

    if (out_col < height && out_row < width) {
        // Read from tile with swapped indices to achieve transpose
        output[out_row * height + out_col] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Using 32x32 threads per block to maximize occupancy and utilize shared memory effectively
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}
