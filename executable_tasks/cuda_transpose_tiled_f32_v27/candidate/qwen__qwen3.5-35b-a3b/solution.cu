#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 16;
constexpr int kBlockRows = 16;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[16][16];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    }

    __syncthreads();

    int out_x = blockIdx.x * kTileDim + threadIdx.x;
    int out_y = blockIdx.y * kTileDim + threadIdx.y;

    if (out_x < height && out_y < width) {
        output[out_x * height + out_y] = tile[threadIdx.x][threadIdx.y];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}