#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_optimized_kernel(const float* input, float* output, int width, int height) {
    extern __shared__ float shared_mem[];
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int x = blockIdx.x * TILE_DIM + tx;
    int y = blockIdx.y * TILE_DIM + ty;

    // Load input tile into shared memory
    if (x < width && y < height) {
        shared_mem[ty * TILE_DIM + tx] = input[y * width + x];
    }
    __syncthreads();

    // Write transposed tile to output
    if (x < height && y < width) {
        output[x * height + y] = shared_mem[ty * TILE_DIM + tx];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(TILE_DIM, TILE_DIM);
    const dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    size_t shared_mem_size = TILE_DIM * TILE_DIM * sizeof(float);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_optimized_kernel<<<grid, block, shared_mem_size>>>(input, output, width, height);
    }
}