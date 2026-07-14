#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM   = 32;
constexpr int BLOCK_ROWS = 8;

__global__ void transpose_tiled(const float * __restrict__ idata,
                                float       * __restrict__ odata,
                                int width, int height)
{
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];   // +1 avoids bank conflicts

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    /* read tile into shared memory (coalesced) */
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < width && (y + j) < height) {
            tile[threadIdx.y + j][threadIdx.x] = idata[(y + j) * width + x];
        }
    }
    __syncthreads();

    /* transpose coordinates */
    x = blockIdx.y * TILE_DIM + threadIdx.x;
    y = blockIdx.x * TILE_DIM + threadIdx.y;

    /* write tile back (coalesced) */
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        if (x < height && (y + j) < width) {
            odata[(y + j) * height + x] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

/* fast path for square matrices: 64×16 tiles (32×32 warps) */
__global__ void transpose_square(const float * __restrict__ idata,
                                 float       * __restrict__ odata,
                                 int n)
{
    __shared__ float tile[64][64 + 1];

    int x = blockIdx.x * 64 + threadIdx.x;
    int y = blockIdx.y * 64 + threadIdx.y;

    /* read 64×64 tile */
    if (x < n && y < n) {
        for (int j = 0; j < 64; j += 16) {
            tile[threadIdx.y + j][threadIdx.x] = idata[(y + j) * n + x];
        }
    }
    __syncthreads();

    x = blockIdx.y * 64 + threadIdx.x;
    y = blockIdx.x * 64 + threadIdx.y;

    if (x < n && y < n) {
        for (int j = 0; j < 64; j += 16) {
            odata[(y + j) * n + x] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters)
{
    if (width == height) {
        const int n = width;
        dim3 block(64, 16);
        dim3 grid((n + 63) / 64, (n + 63) / 64);
        for (int i = 0; i < iters; ++i) {
            transpose_square<<<grid, block>>>(input, output, n);
        }
    } else {
        dim3 block(TILE_DIM, BLOCK_ROWS);
        dim3 grid((width  + TILE_DIM - 1) / TILE_DIM,
                  (height + TILE_DIM - 1) / TILE_DIM);
        for (int i = 0; i < iters; ++i) {
            transpose_tiled<<<grid, block>>>(input, output, width, height);
        }
    }
}