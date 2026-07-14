#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDim + 1];

    const int block_x = blockIdx.x * kTileDim;
    const int block_y = blockIdx.y * kTileDim;
    const int thread_x = threadIdx.x;
    const int thread_y = threadIdx.y;

    // Load input tile to shared memory with coalesced global reads
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int global_x = block_x + thread_x;
        const int global_y = block_y + thread_y + j;
        if (global_x < width && global_y < height) {
            tile[thread_y + j][thread_x] = input[global_y * width + global_x];
        }
    }

    __syncthreads();

    // Write transposed tile to global memory with coalesced writes
    const int transposed_block_x = block_y;
    const int transposed_block_y = block_x;
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int global_x = transposed_block_x + thread_x;
        const int global_y = transposed_block_y + thread_y + j;
        if (global_x < height && global_y < width) {
            output[global_y * height + global_x] = tile[thread_x][thread_y + j];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}