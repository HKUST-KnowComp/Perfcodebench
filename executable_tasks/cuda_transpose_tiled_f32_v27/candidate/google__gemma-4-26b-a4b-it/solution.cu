#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding the shared memory dimension to avoid bank conflicts
constexpr int kTileDimPadded = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDimPadded][kTileDim];

    // Calculate global coordinates
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load data into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new global coordinates for transposed output
    // The output index is (x, y) in the transposed space, which corresponds to (y, x) in original
    // However, we want to write to output[y_new * width_new + x_new]
    // where width_new = height and height_new = width.
    // To ensure coalesced writes, we map threadIdx.x to the output's column index.
    
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    if (out_x < height && out_y < width) {
        // Read from shared memory (transposed access)
        // tile[threadIdx.x][threadIdx.y] provides coalesced access to the output
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to match the tile dimension
    // This allows each thread to handle one element per tile
    dim3 block(kTileDim, kTileDim);
    
    // Grid covers the input dimensions
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
    cudaDeviceSynchronize();
}