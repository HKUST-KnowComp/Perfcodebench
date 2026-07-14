#include "interface.h"
#include <cuda_runtime.h>

constexpr int TILE = 32;
constexpr int BLK_Y = 8;
__global__ void transpose_kernel(const float* in, float* out, int w, int h, int iters) {
    __shared__ float tile[TILE][TILE + 1]; // +1 avoids bank conflicts
    int x = blockIdx.x * TILE + threadIdx.x;
    int y = blockIdx.y * TILE + threadIdx.y;
    int sx = blockIdx.y * TILE + threadIdx.x; // swapped indices for transposed write
    int sy = blockIdx.x * TILE + threadIdx.y;
    for (int iter = 0; iter < iters; ++iter) {
        // cooperative load into shared
        if (x < w && y < h)
            tile[threadIdx.y][threadIdx.x] = in[y * w + x];
        __syncthreads();
        // cooperative store back, reordered
        if (sx < h && sy < w)
            out[sy * h + sx] = tile[threadIdx.x][threadIdx.y];
        __syncthreads();
    }
}

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE, BLK_Y, 1);
    dim3 grid((width + TILE - 1) / TILE, (height + TILE - 1) / TILE, 1);
    transpose_kernel<<<grid, block>>>(input, output, width, height, iters);
}