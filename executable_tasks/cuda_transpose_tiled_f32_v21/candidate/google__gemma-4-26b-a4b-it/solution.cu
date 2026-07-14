#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int col = blockIdx.x * kTileDim + tx;
    int row = blockIdx.y * kTileDim + ty;

    // Read from input into shared memory
    if (row < height && col < width) {
        tile[ty][tx] = input[row * width + col];
    } else {
        tile[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Calculate new coordinates for transposed output
    // The output index is (col, row) in the transposed matrix
    // Transposed width is the original height
    int out_col = blockIdx.y * kTileDim + ty;
    int out_row = blockIdx.x * kTileDim + tx;

    if (out_row < width && out_col < height) {
        // Write from shared memory to output (coalesced)
        output[out_row * height + out_col] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to match the tile dimension
    // The grid dimensions are calculated to cover the entire input matrix
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}