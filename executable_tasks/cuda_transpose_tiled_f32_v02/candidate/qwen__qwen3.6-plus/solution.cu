#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    for (int i = 0; i < kTileDim / kBlockRows; ++i) {
        int row = y + i * kBlockRows;
        if (row < height && x < width) {
            tile[threadIdx.y + i * kBlockRows][threadIdx.x] = input[row * width + x];
        }
    }
    __syncthreads();

    for (int i = 0; i < kTileDim / kBlockRows; ++i) {
        int out_x = blockIdx.y * kTileDim + threadIdx.x;
        int out_y = blockIdx.x * kTileDim + threadIdx.y + i * kBlockRows;
        if (out_x < width && out_y < height) {
            output[out_x * height + out_y] = tile[threadIdx.y + i * kBlockRows][threadIdx.x];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}
