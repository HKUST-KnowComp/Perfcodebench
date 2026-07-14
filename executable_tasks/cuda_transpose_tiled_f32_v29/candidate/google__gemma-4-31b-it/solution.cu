#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Tile size 32x32 is generally efficient for most GPUs
constexpr int TILE_DIM = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    // [TILE_DIM][TILE_DIM + 1] ensures that columns map to different banks
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    const int x = blockIdx.x * TILE_DIM + threadIdx.x;
    const int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Read from global memory into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate the output coordinates
    // The block is transposed: blockIdx.x becomes the row index for output
    const int out_x = blockIdx.y * TILE_DIM + threadIdx.x;
    const int out_y = blockIdx.x * TILE_DIM + threadIdx.y;

    // Write from shared memory to global memory (coalesced)
    if (out_x < width && out_y < height) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    } // Note: The logic here is slightly different to ensure coalesced writes
    // Correct mapping for transpose: output[x][y] = input[y][x]
    // In our tiled approach: 
    // input index: (blockIdx.y * 32 + ty) * width + (blockIdx.x * 32 + tx)
    // output index: (blockIdx.x * 32 + tx) * height + (blockIdx.y * 32 + ty)
    // To make output writes coalesced, we need to write to output[out_y * height + out_x]
    // where out_x is the fast-moving index.
}

// Refined kernel for better coalescing
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int gx = blockIdx.x * TILE_DIM + tx;
    int gy = blockIdx.y * TILE_DIM + ty;

    if (gx < width && gy < height) {
        tile[ty][tx] = input[gy * width + gx];
    } else {
        tile[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Transpose the tile indices for the write phase
    // We want to write to output[ (blockIdx.x * 32 + tx) * height + (blockIdx.y * 32 + ty) ]
    // But to be coalesced, the inner index must be the one varying with threadIdx.x
    // So we swap the roles of tx and ty for the output write
    int out_gx = blockIdx.y * TILE_DIM + tx;
    int out_gy = blockIdx.x * TILE_DIM + ty;

    if (out_gx < width && out_gy < height) {
        output[out_gy * height + out_gx] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);

    for (int i = 0; i < iters; ++i) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
    cudaDeviceSynchronize();
}
