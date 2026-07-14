#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int block_x = blockIdx.x;
    int block_y = blockIdx.y;

    int input_x = block_x * kTileDim + tx;
    int input_y = block_y * kTileDim + ty;

    if (input_x < width && input_y < height) {
        tile[tx][ty] = input[input_y * width + input_x];
    }

    __syncthreads();

    int output_x = input_x;
    int output_y = input_y;

    if (output_x < height && output_y < width) {
        output[output_x * height + output_y] = tile[tx][ty];
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