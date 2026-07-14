#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
    extern __shared__ float shared_mem[];
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int block_x = blockIdx.x;
    int block_y = blockIdx.y;
    int tile_x = block_x * TILE_DIM;
    int tile_y = block_y * TILE_DIM;

    int in_x = tile_x + tx;
    int in_y = tile_y + ty;

    // Load input into shared memory
    if (in_x < width && in_y < height) {
        shared_mem[ty * TILE_DIM + tx] = input[in_y * width + in_x];
    }
    __syncthreads();

    // Transpose and write to output
    int out_x = tile_x + ty;
    int out_y = tile_y + tx;

    if (out_x < width && out_y < height) {
        output[out_x * height + out_y] = shared_mem[tx * TILE_DIM + ty];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block, TILE_DIM * TILE_DIM * sizeof(float)>>>(input, output, width, height);
    }
}