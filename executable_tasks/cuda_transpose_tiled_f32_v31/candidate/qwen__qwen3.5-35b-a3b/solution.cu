#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
    extern __shared__ float shared_mem[];
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int block_x = blockIdx.x;
    const int block_y = blockIdx.y;

    const int in_row = block_y * TILE_DIM + ty;
    const int in_col = block_x * TILE_DIM + tx;

    if (in_row < height && in_col < width) {
        shared_mem[ty * TILE_DIM + tx] = input[in_row * width + in_col];
    }

    __syncthreads();

    const int out_row = block_x * TILE_DIM + tx;
    const int out_col = block_y * TILE_DIM + ty;

    if (out_row < width && out_col < height) {
        output[out_row * height + out_col] = shared_mem[ty * TILE_DIM + tx];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block, TILE_DIM * TILE_DIM * sizeof(float)>>>(input, output, width, height);
    }
}