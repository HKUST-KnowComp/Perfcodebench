#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__ in,
                                       float* __restrict__ out,
                                       int width, int height)
{
    __shared__ float tile[TILE][TILE + 1];  // +1 padding to avoid bank conflicts

    int x = blockIdx.x * TILE + threadIdx.x;
    int y = blockIdx.y * TILE + threadIdx.y;

    // Load tile into shared memory (coalesced)
    if (x < width && y < height)
        tile[threadIdx.y][threadIdx.x] = in[y * width + x];

    __syncthreads();

    // Transpose coordinates for output
    x = blockIdx.y * TILE + threadIdx.x;
    y = blockIdx.x * TILE + threadIdx.y;

    // Store transposed tile (coalesced)
    if (x < height && y < width)
        out[y * height + x] = tile[threadIdx.x][threadIdx.y];
}

}  // namespace

void transpose_f32(const float* input, float* output,
                   int width, int height, int iters)
{
    dim3 block(TILE, TILE);
    dim3 grid((width + TILE - 1) / TILE, (height + TILE - 1) / TILE);

    for (int i = 0; i < iters; ++i)
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
}