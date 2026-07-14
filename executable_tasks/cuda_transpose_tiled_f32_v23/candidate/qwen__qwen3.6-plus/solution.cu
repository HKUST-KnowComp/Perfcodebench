#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    int in_col = blockIdx.x * kTileDim + threadIdx.x;
    int in_row_base = blockIdx.y * kTileDim + threadIdx.y;

    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        int in_row = in_row_base + j;
        if (in_row < height && in_col < width) {
            tile[threadIdx.y + j][threadIdx.x] = input[in_row * width + in_col];
        }
    }
    __syncthreads();

    int out_row_base = blockIdx.x * kTileDim + threadIdx.y;
    int out_col = blockIdx.y * kTileDim + threadIdx.x;

    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        int out_row = out_row_base + j;
        if (out_row < width && out_col < height) {
            output[out_row * height + out_col] = tile[threadIdx.x][threadIdx.y + j];
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
