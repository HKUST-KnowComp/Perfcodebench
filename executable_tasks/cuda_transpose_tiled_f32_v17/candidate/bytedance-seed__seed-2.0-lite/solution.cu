#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
    // Shared memory tile padded by 1 element to eliminate shared memory bank conflicts
    __shared__ float tile[kTileDim][kTileDim + 1];
    
    const int block_x = blockIdx.x * kTileDim;
    const int block_y = blockIdx.y * kTileDim;
    
    // Load input tile into shared memory with coalesced global reads
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int x = block_x + threadIdx.x;
        const int y = block_y + threadIdx.y + j;
        if (x < width && y < height) {
            tile[threadIdx.y + j][threadIdx.x] = input[y * width + x];
        }
    }
    
    // Synchronize to ensure all tile loads are complete before writes
    __syncthreads();
    
    // Write transposed tile to global memory with coalesced global writes
    const int transposed_block_x = block_y;
    const int transposed_block_y = block_x;
    for (int j = 0; j < kTileDim; j += kBlockRows) {
        const int x = transposed_block_x + threadIdx.x;
        const int y = transposed_block_y + threadIdx.y + j;
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
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}