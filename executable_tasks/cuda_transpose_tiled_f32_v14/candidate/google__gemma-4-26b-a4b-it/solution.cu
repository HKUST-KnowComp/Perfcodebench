#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding to avoid shared memory bank conflicts
constexpr int kTileDimPadded = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDimPadded][kTileDimPadded];

    // Calculate global indices
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

    // Calculate new global indices for the transposed output
    // The output coordinates are swapped: new_x = old_y, new_y = old_x
    // However, we want to write to output in a coalesced manner.
    // The blockIdx.x/y mapping is swapped to ensure coalesced writes.
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    if (out_x < height && out_y < width) {
        // Read from shared memory using transposed indices to ensure coalesced writes
        // tile[threadIdx.x][threadIdx.y] corresponds to the transposed position
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more robust version using a standard 32x32 block mapping
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDimPadded];

    int col = blockIdx.x * kTileDim + threadIdx.x;
    int row = blockIdx.y * kTileDim + threadIdx.y;

    if (col < width && row < height) {
        tile[threadIdx.y][threadIdx.x] = input[row * width + col];
    }

    __syncthreads();

    // Transpose the indices for writing
    // The output block coordinates are swapped to maintain coalesced writes
    int out_col = blockIdx.y * kTileDim + threadIdx.x;
    int out_row = blockIdx.x * kTileDim + threadIdx.y;

    if (out_col < height && out_row < width) {
        output[out_row * height + out_col] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Using a 32x32 block for maximum occupancy and coalescing efficiency
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        // We use the optimized kernel with shared memory tiling
        // Note: The grid dimensions are calculated based on input dimensions
        // but the kernel logic handles the swap for the output dimensions.
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}
