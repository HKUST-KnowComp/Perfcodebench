#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 16;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim];
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x;
    int by = blockIdx.y;

    int x = bx * kTileDim + tx;
    int y = by * kTileDim + ty;

    if (x < width && y < height) {
        tile[ty][tx] = input[y * width + x];
    }

    __syncthreads();

    x = by * kTileDim + ty;
    y = bx * kTileDim + tx;

    if (x < width && y < height) {
        output[x * height + y] = tile[ty][tx];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}