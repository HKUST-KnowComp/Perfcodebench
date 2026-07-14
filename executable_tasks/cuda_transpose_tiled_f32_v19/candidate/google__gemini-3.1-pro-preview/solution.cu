#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    // Shared memory tile with padding to avoid bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];

    // Calculate global coordinates for reading
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Read from input into shared memory (coalesced)
    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[(y + j) * width + x];
        }
    }

    __syncthreads();

    // Transpose block indices for the output write
    // New x and y coordinates for coalesced writing to the output matrix
    int x_out = blockIdx.y * kTileDim + threadIdx.x;
    int y_out = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output (coalesced)
    #pragma unroll
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        if (x_out < height && (y_out + j) < width) {
            output[(y_out + j) * height + x_out] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    if (width <= 0 || height <= 0) return;

    const dim3 block(kTileDim, kBlockRows);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);

    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}