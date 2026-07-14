#include "interface.h"

#include <cuda_runtime.h>

namespace {

// 32×32 tile, 1024 threads per block
constexpr int TILE = 32;

__launch_bounds__(TILE * TILE, 1)
__global__ void transpose_tiled(const float* __restrict__ in,
                                float* __restrict__ out,
                                int width, int height)
{
    __shared__ float tile[TILE][TILE + 1];  // +1 to avoid bank conflicts

    int x = blockIdx.x * TILE + threadIdx.x;
    int y = blockIdx.y * TILE + threadIdx.y;

    // Load into shared memory (coalesced)
    if (x < width && y < height)
        tile[threadIdx.y][threadIdx.x] = in[y * width + x];

    __syncthreads();

    // Transpose coordinates for output
    int outX = blockIdx.y * TILE + threadIdx.x;
    int outY = blockIdx.x * TILE + threadIdx.y;

    // Store transposed tile (coalesced)
    if (outX < height && outY < width)
        out[outY * height + outX] = tile[threadIdx.x][threadIdx.y];
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters)
{
    dim3 block(TILE, TILE);
    dim3 grid((width + TILE - 1) / TILE, (height + TILE - 1) / TILE);

    for (int i = 0; i < iters; ++i)
        transpose_tiled<<<grid, block>>>(input, output, width, height);
}