#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;
constexpr int BLOCK_ROWS = 8;   // 32×8 threads per block

__global__ __launch_bounds__(TILE_DIM* BLOCK_ROWS)
void transpose_tiled_kernel(const float* __restrict__ idata,
                            float* __restrict__ odata,
                            int width, int height)
{
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];   // +1 avoids bank conflicts

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;

    // Load tile into shared memory (coalesced)
    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        int yy = y + j;
        if (x < width && yy < height) {
            tile[threadIdx.y + j][threadIdx.x] = idata[yy * width + x];
        }
    }

    __syncthreads();

    // Transposed indices for writing
    x = blockIdx.y * TILE_DIM + threadIdx.x;
    y = blockIdx.x * TILE_DIM + threadIdx.y;

    // Store transposed tile (coalesced)
    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS) {
        int yy = y + j;
        if (x < height && yy < width) {
            odata[yy * height + x] = tile[threadIdx.x][threadIdx.y + j];
        }
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(TILE_DIM, BLOCK_ROWS);
    const dim3 grid((width  + TILE_DIM - 1) / TILE_DIM,
                    (height + TILE_DIM - 1) / TILE_DIM);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}