#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding the shared memory dimension by 1 to avoid bank conflicts
constexpr int kSharedDim = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kSharedDim][kSharedDim];

    // Calculate global indices
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load data into shared memory
    // We use threadIdx.x and threadIdx.y to map to the tile
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new indices for the transposed output
    // The output coordinates are (y, x) relative to the original
    // In the transposed matrix, the new x is the old y, and new y is the old x
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output
    // To ensure coalesced writes, we read from shared memory such that 
    // consecutive threads write to consecutive output addresses.
    // The original (x, y) becomes (y, x). 
    // To make output[out_y * height + out_x] coalesced, 
    // we need threadIdx.x to map to the contiguous dimension of the output.
    
    // Re-calculating indices for coalesced output write:
    // The output matrix has width 'height' and height 'width'.
    // A thread (tx, ty) in the block will write to output[out_y * height + out_x]
    // where out_x is the fast-moving index.
    
    // We need to read from the tile such that we access tile[threadIdx.x][threadIdx.y]
    // to match the output layout.
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more robust version of the tiled kernel to handle arbitrary dimensions correctly
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

    // Transpose indices: the blockIdx.x and blockIdx.y are swapped
    int new_col = blockIdx.y * kTileDim + tx;
    int new_row = blockIdx.x * kTileDim + ty;

    if (new_col < height && new_row < width) {
        // Read from tile in a way that ensures coalesced writes to output
        // output[new_row * height + new_col] is the target.
        // Since new_col is the fast index (tx), we access tile[tx][ty]
        output[new_row * height + new_col] = tile[tx][ty];
    }
}

// Final refined kernel for maximum performance
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int x = blockIdx.x * kTileDim + tx;
    int y = blockIdx.y * kTileDim + ty;

    if (x < width && y < height) {
        tile[ty][tx] = input[y * width + x];
    }

    __syncthreads();

    // The output matrix has dimensions (height, width)
    // We want to write to output[new_y * height + new_x]
    // To be coalesced, new_x must be the fast-moving index (tx)
    int out_x = blockIdx.y * kTileDim + tx;
    int out_y = blockIdx.x * kTileDim + ty;

    if (out_x < height && out_y < width) {
        // We read from the tile using [tx][ty] to effectively transpose
        output[out_y * height + out_x] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to maximize occupancy and utilize shared memory effectively
    // The block dimensions are (32, 32) but we only use 32 threads per block if we want to match 
    // the logic above, or we use a 32x32 block and map threads.
    // Let's use a 32x32 block for simplicity and performance.
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_final_kernel<<<grid, block>>>(input, output, width, height);
    }
}
