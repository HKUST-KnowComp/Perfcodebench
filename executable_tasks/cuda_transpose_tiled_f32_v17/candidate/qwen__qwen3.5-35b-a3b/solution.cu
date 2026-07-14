#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim];
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load into shared memory
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }
    __syncthreads();

    // Transpose and write to output
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int out_x = blockIdx.x * kTileDim + tx;
    int out_y = blockIdx.y * kTileDim + ty;

    if (out_x < width && out_y < height) {
        output[out_x * height + out_y] = tile[ty][tx];
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