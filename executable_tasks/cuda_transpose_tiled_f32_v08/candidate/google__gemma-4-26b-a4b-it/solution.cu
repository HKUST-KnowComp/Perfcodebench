#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding to avoid shared memory bank conflicts during transposed access
constexpr int kTileDimPadded = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDimPadded][kTileDimPadded];

    // Calculate global coordinates
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load from input into shared memory (coalesced read)
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new global coordinates for transposed output
    // The output index for input[y][x] is output[x][y]
    // New x is the old y, new y is the old x
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output (coalesced write)
    // We read from tile[threadIdx.x][threadIdx.y] to transpose
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to match the tile dimension
    // Note: The kernel logic above uses threadIdx.x/y to map to the tile
    // To ensure we cover the whole tile, we use a 32x32 block.
    // However, the original kernel used kBlockRows=8. 
    // For maximum performance, a full 32x32 block is preferred.
    
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
    cudaDeviceSynchronize();
}