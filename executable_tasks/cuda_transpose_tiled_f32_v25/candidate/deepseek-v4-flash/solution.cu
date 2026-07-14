#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_shared_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim];

    // Global coordinates for input
    int x_in = blockIdx.x * kTileDim + threadIdx.x;
    int y_in = blockIdx.y * kTileDim + threadIdx.y;

    // Coalesced read from global to shared
    if (x_in < width && y_in < height) {
        tile[threadIdx.y][threadIdx.x] = input[y_in * width + x_in];
    }
    __syncthreads();

    // Global coordinates for output (transposed)
    int x_out = blockIdx.y * kTileDim + threadIdx.x;  // column index in output
    int y_out = blockIdx.x * kTileDim + threadIdx.y;  // row index in output

    // Coalesced write from shared to global
    if (x_out < height && y_out < width) {
        output[y_out * height + x_out] = tile[threadIdx.x][threadIdx.y];
    }
}

}  // anonymous namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_shared_kernel<<<grid, block>>>(input, output, width, height);
    }
}
