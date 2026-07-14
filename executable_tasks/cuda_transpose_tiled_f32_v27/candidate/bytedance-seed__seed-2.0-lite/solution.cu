#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with 1-element padding to eliminate shared memory bank conflicts
    __shared__ float tile[33][32];

    const int block_base_x = blockIdx.x * kTileDim;
    const int block_base_y = blockIdx.y * kTileDim;

    // Load input tile into shared memory with coalesced global reads
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int y = block_base_y + threadIdx.y + j;
        const int x = block_base_x + threadIdx.x;
        if (x < width && y < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[y * width + x];
        }
    }

    // Synchronize to ensure full tile is loaded before writing
    __syncthreads();

    // Write transposed tile to output with coalesced global writes
    const int transposed_base_x = blockIdx.y * kTileDim;
    const int transposed_base_y = blockIdx.x * kTileDim;

    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int y = transposed_base_y + threadIdx.y + j;
        const int x = transposed_base_x + threadIdx.x;
        if (x < height && y < width) {
            output[y * height + x] = tile[threadIdx.x][threadIdx.y + j];
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