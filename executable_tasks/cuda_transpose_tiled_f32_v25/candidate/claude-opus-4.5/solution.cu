#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;
constexpr int BLOCK_ROWS = 8;

__global__ void transpose_shared_kernel(const float* __restrict__ input, 
                                         float* __restrict__ output, 
                                         int width, int height) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];
    
    int xIndex = blockIdx.x * TILE_DIM + threadIdx.x;
    int yIndex = blockIdx.y * TILE_DIM + threadIdx.y;
    
    int index_in = xIndex + yIndex * width;
    
    int xIndexOut = blockIdx.y * TILE_DIM + threadIdx.x;
    int yIndexOut = blockIdx.x * TILE_DIM + threadIdx.y;
    
    int index_out = xIndexOut + yIndexOut * height;
    
    #pragma unroll
    for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
        if (xIndex < width && (yIndex + i) < height) {
            tile[threadIdx.y + i][threadIdx.x] = input[index_in + i * width];
        }
    }
    
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
        if (xIndexOut < height && (yIndexOut + i) < width) {
            output[index_out + i * height] = tile[threadIdx.x][threadIdx.y + i];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, BLOCK_ROWS);
    dim3 grid((width + TILE_DIM - 1) / TILE_DIM, (height + TILE_DIM - 1) / TILE_DIM);
    
    for (int iter = 0; iter < iters; ++iter) {
        transpose_shared_kernel<<<grid, block>>>(input, output, width, height);
    }
}
