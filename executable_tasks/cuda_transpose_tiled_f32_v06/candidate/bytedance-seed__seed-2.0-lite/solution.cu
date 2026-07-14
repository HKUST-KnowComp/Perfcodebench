#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
    // Padded shared memory to eliminate shared memory bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int x = blockIdx.x * kTileDim + tx;
    const int y_base = blockIdx.y * kTileDim + ty;

    // Load input tile into shared memory with coalesced global reads
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int y = y_base + j;
        if (x < width && y < height) {
            tile[ty + j][tx] = input[y * width + x];
        }
    }

    // Synchronize all threads in block to ensure tile is fully loaded
    __syncthreads();

    // Write transposed tile to output with coalesced global writes
    const int x_out_base = blockIdx.y * kTileDim + tx;
    const int y_out_base = blockIdx.x * kTileDim + ty;
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int x_out = x_out_base;
        const int y_out = y_out_base + j;
        if (x_out < height && y_out < width) {
            output[y_out * height + x_out] = tile[tx][ty + j];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}