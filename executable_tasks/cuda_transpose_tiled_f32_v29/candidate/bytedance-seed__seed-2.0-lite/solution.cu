#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared tile with +1 padding to eliminate shared memory bank conflicts
    __shared__ float tile[kBlockRows][kTileDim + 1];

    // Calculate input coordinates for current thread
    const int x = blockIdx.x * kTileDim + threadIdx.x;
    const int y = blockIdx.y * kBlockRows + threadIdx.y;

    // Coalesced load of tile from global to shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }

    // Synchronize to ensure full tile is loaded before writes
    __syncthreads();

    // Calculate transposed output coordinates
    const int out_x = blockIdx.y * kBlockRows + threadIdx.y;
    const int out_y = blockIdx.x * kTileDim + threadIdx.x;

    // Coalesced write of transposed tile to global memory
    if (out_x < height && out_y < width) {
        output[out_x * width + out_y] = tile[threadIdx.y][threadIdx.x];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kBlockRows - 1) / kBlockRows);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}