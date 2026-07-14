#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

// Optimized transpose kernel using shared memory to coalesce global memory accesses.
// Shared memory padding (kTileDim + 1) is used to avoid bank conflicts.
__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    // Calculate global coordinates for the input matrix
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load data from global memory into shared memory
    // Each thread in the block processes multiple elements in the tile
    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
        }
    }

    __syncthreads();

    // Calculate global coordinates for the output matrix (transposed)
    // We swap blockIdx.x and blockIdx.y to transpose the tile position
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Store data from shared memory into global memory
    // Accessing tile[threadIdx.x][threadIdx.y + j] transposes the data within the tile
    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        if (out_x < height && (out_y + j) < width) {
            output[(out_y + j) * height + out_x] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    if (width <= 0 || height <= 0) return;

    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}