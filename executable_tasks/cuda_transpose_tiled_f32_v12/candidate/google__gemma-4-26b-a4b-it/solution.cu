#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding to avoid bank conflicts in shared memory
constexpr int kTileDimPadded = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDimPadded][kTileDimPadded];

    // Calculate global indices
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load from input into shared memory (coalesced read)
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new indices for transposed output
    // The output coordinates are (y, x) relative to the original matrix
    // In the transposed matrix, the new x is the old y, and new y is the old x
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output (coalesced write)
    // We read from tile[threadIdx.x][threadIdx.y] to ensure the write to output is coalesced
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more efficient version using a single thread to handle multiple elements
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDimPadded];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int col = blockIdx.x * kTileDim + tx;
    int row = blockIdx.y * kTileDim + ty;

    if (col < width && row < height) {
        tile[ty][tx] = input[row * width + col];
    }

    __syncthreads();

    // Transpose indices: blockIdx.x and blockIdx.y are swapped
    int new_col = blockIdx.y * kTileDim + tx;
    int new_row = blockIdx.x * kTileDim + ty;

    if (new_col < height && new_row < width) {
        output[new_row * height + new_col] = tile[tx][ty];
    }
}

// Final high-performance kernel using 16x16 threads to cover 32x32 tiles
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDimPadded];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    
    // Each thread handles 2 elements to fill a 32x32 tile with 16x16 threads
    int x = blockIdx.x * kTileDim + tx + (ty >= 16 ? 16 : 0);
    int y = blockIdx.y * kTileDim + ty;

    // Load first element
    if (x < width && y < height) {
        tile[ty][tx] = input[y * width + x];
        tile[ty][tx + 16] = input[y * width + x + 16]; // This logic is slightly flawed for non-power-of-2, let's use standard 32x32
    }
}

// Standard robust tiled kernel
__global__ void transpose_robust_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDimPadded];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int x = blockIdx.x * kTileDim + tx;
    int y = blockIdx.y * kTileDim + ty;

    if (x < width && y < height) {
        tile[ty][tx] = input[y * width + x];
    }

    __syncthreads();

    // Transpose: output[x][y] = input[y][x]
    // To make output write coalesced, we want threadIdx.x to map to the contiguous dimension of output
    // Output is height x width. Transposed output is width x height.
    // New width is 'height', new height is 'width'.
    int out_x = blockIdx.y * kTileDim + tx;
    int out_y = blockIdx.x * kTileDim + ty;

    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Using 32x32 threads for a 32x32 tile
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        // We use the robust kernel which is highly optimized for coalescing
        // and bank conflict avoidance via padding.
        transpose_robust_kernel<<<grid, block>>>(input, output, width, height);
    }
}
