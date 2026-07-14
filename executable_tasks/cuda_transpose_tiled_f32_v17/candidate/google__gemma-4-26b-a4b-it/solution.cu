#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load from input into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }

    __syncthreads();

    // Calculate new coordinates for transposed output
    // The output coordinates are (y, x) relative to the original matrix
    // In the transposed matrix, the new x is the old y, and new y is the old x
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output
    // We use the threadIdx to ensure coalesced writes to the output matrix
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to match the tile dimension
    // This allows us to load a full tile and write it back transposed
    dim3 block(kTileDim, kTileDim);
    
    // Grid covers the input dimensions
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}