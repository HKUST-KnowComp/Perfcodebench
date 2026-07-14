#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding the shared memory dimension to avoid bank conflicts
constexpr int kSharedDim = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kSharedDim][kSharedDim];

    // Calculate global coordinates
    int col = blockIdx.x * kTileDim + threadIdx.x;
    int row = blockIdx.y * kTileDim + threadIdx.y;

    // 1. Coalesced Read from Input into Shared Memory
    if (col < width && row < height) {
        tile[threadIdx.y][threadIdx.x] = input[row * width + col];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // 2. Calculate new coordinates for Transposed Output
    // The output index for (row, col) in a transpose is (col, row)
    // New row = col, New col = row
    int out_col = blockIdx.y * kTileDim + threadIdx.x;
    int out_row = blockIdx.x * kTileDim + threadIdx.y;

    // 3. Coalesced Write to Output from Shared Memory
    // We read from tile[threadIdx.x][threadIdx.y] to ensure coalesced writes
    if (out_col < height && out_row < width) {
        output[out_row * height + out_col] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // We use a 32x32 block to match the tile dimension
    // This allows for maximum coalescing efficiency
    dim3 block(kTileDim, kTileDim);
    dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
    cudaDeviceSynchronize();
}