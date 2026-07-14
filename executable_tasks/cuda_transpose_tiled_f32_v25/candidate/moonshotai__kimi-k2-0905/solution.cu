#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;          // width  of tile (and thread block)
constexpr int BLOCK_ROWS = 8;         // height of thread block
constexpr int NUM_ROWS_PER_THREAD = 4; // each thread handles 4 rows => 32×32 tile

__global__ void transpose_tiled_kernel(const float* __restrict__ in,
                                       float*       __restrict__ out,
                                       int width, int height)
{
    __shared__ float tile[TILE_DIM][TILE_DIM + 1]; // +1 removes bank conflicts

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y0 = blockIdx.y * TILE_DIM + threadIdx.y * NUM_ROWS_PER_THREAD;

    // Read into shared memory with coalesced, aligned accesses
    #pragma unroll
    for (int r = 0; r < NUM_ROWS_PER_THREAD; ++r) {
        int y = y0 + r;
        if (x < width && y < height) {
            tile[threadIdx.y * NUM_ROWS_PER_THREAD + r][threadIdx.x] = in[y * width + x];
        }
    }
    __syncthreads();

    // Transpose indices
    x = blockIdx.y * TILE_DIM + threadIdx.x;
    y0 = blockIdx.x * TILE_DIM + threadIdx.y * NUM_ROWS_PER_THREAD;

    // Write back with coalesced, aligned accesses
    #pragma unroll
    for (int r = 0; r < NUM_ROWS_PER_THREAD; ++r) {
        int y = y0 + r;
        if (x < height && y < width) {
            out[y * height + x] = tile[threadIdx.x][threadIdx.y * NUM_ROWS_PER_THREAD + r];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, BLOCK_ROWS);
    dim3 grid((width  + TILE_DIM - 1) / TILE_DIM,
              (height + TILE_DIM - 1) / TILE_DIM);

    for (int i = 0; i < iters; ++i) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
