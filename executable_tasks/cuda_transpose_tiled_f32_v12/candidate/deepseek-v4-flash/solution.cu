#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
    // Shared memory tile with padding to reduce bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Coalesced global load from input
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }
    __syncthreads();

    // Transposed write to output with coalesced stores
    int x_out = blockIdx.y * kTileDim + threadIdx.x;   // column in output (original row)
    int y_out = blockIdx.x * kTileDim + threadIdx.y;   // row in output (original column)
    if (x_out < height && y_out < width) {
        output[y_out * height + x_out] = tile[threadIdx.x][threadIdx.y];
    }
}

}  // anonymous namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
