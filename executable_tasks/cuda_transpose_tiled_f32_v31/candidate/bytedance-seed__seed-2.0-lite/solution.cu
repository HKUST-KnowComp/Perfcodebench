#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
    // Shared memory tile with +1 column padding to eliminate shared memory bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];
    
    // Calculate input coordinates for the current thread
    const int x_in = blockIdx.x * kTileDim + threadIdx.x;
    const int y_in = blockIdx.y * kTileDim + threadIdx.y;
    
    // Load tile from global memory (fully coalesced read operation)
    if (x_in < width && y_in < height) {
        tile[threadIdx.y][threadIdx.x] = input[y_in * width + x_in];
    }
    
    // Synchronize all threads in the block to ensure tile is fully loaded
    __syncthreads();
    
    // Calculate transposed output coordinates
    const int x_out = blockIdx.y * kTileDim + threadIdx.x;
    const int y_out = blockIdx.x * kTileDim + threadIdx.y;
    
    // Write transposed tile to global memory (fully coalesced write operation)
    if (x_out < height && y_out < width) {
        output[y_out * height + x_out] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block>>>(input, output, width, height);
    }
}