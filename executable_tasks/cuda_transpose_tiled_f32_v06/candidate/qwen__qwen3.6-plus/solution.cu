#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];
    
    int bx = blockIdx.x * TILE_DIM;
    int by = blockIdx.y * TILE_DIM;
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    
    int row = by + ty;
    int col = bx + tx;
    
    if (row < height && col < width) {
        tile[ty][tx] = input[row * width + col];
    }
    __syncthreads();
    
    int out_row = col;
    int out_col = row;
    if (out_row < width && out_col < height) {
        output[out_row * height + out_col] = tile[tx][ty];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    
    for (int iter = 0; iter < iters; ++iter) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}
