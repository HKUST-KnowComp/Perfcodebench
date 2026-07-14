#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared tile with +1 padding to eliminate shared memory bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    // Calculate input coordinates for current thread
    const int x = blockIdx.x * kTileDim + threadIdx.x;
    const int y = blockIdx.y * kTileDim + threadIdx.y;

    // Coalesced load of input tile from global memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }

    // Synchronize to ensure all tile elements are loaded before writeback
    __syncthreads();

    // Calculate transposed output coordinates
    const int x_out = blockIdx.y * kTileDim + threadIdx.x;
    const int y_out = blockIdx.x * kTileDim + threadIdx.y;

    // Coalesced write of transposed tile to global memory
    if (x_out < height && y_out < width) {
        output[y_out * height + x_out] = tile[threadIdx.x][threadIdx.y];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}