#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
    // Shared memory tile with padding to avoid 32-way bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    // Calculate coordinates in input matrix
    const int in_x = blockIdx.x * kTileDim + threadIdx.x;
    const int in_y = blockIdx.y * kTileDim + threadIdx.y;

    // Load tile from global to shared memory (coalesced input access)
    if (in_x < width && in_y < height) {
        tile[threadIdx.y][threadIdx.x] = input[in_y * width + in_x];
    }
    __syncthreads(); // Ensure all threads finish loading the tile before transposing

    // Calculate coordinates in output (transposed) matrix
    const int out_x = blockIdx.y * kTileDim + threadIdx.x;
    const int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write transposed tile from shared to global memory (coalesced output access)
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}