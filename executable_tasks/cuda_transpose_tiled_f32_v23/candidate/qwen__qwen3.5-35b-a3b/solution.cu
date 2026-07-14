#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_shared_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int x_in = bx * kTileDim + tx;
    int y_in = by * kTileDim + ty;

    // Load input into shared memory
    if (x_in < width && y_in < height) {
        tile[ty][tx] = input[y_in * width + x_in];
    } else {
        tile[ty][tx] = 0.0f;
    }

    __syncthreads();

    // Transpose and write to output
    int x_out = by * kTileDim + tx;
    int y_out = bx * kTileDim + ty;

    if (x_out < width && y_out < height) {
        output[x_out * height + y_out] = tile[tx][ty];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_shared_kernel<<<grid, block>>>(input, output, width, height);
    }
}