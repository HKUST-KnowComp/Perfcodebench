#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int TILE_DIM = 32;

__global__ void transpose_tiled_kernel(const float* __restrict__  in,
                                     float* __restrict__  out,
                                     int width, int height)
{
    __shared__ float tile[TILE_DIM][TILE_DIM];

    int x_in  = blockIdx.x * TILE_DIM + threadIdx.x;
    int y_in  = blockIdx.y * TILE_DIM + threadIdx.y;
    int x_out = blockIdx.y * TILE_DIM + threadIdx.x;
    int y_out = blockIdx.x * TILE_DIM + threadIdx.y;

    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += blockDim.y) {
        int idx_in = (y_in + j) * width + x_in;
        if (x_in < width && (y_in + j) < height)
            tile[threadIdx.y + j][threadIdx.x] = __ldg(&in[idx_in]);
    }

    __syncthreads();

    #pragma unroll
    for (int j = 0; j < TILE_DIM; j += blockDim.y) {
        int idx_out = (y_out + j) * height + x_out;
        if (x_out < height && (y_out + j) < width)
            out[idx_out] = tile[threadIdx.x][threadIdx.y + j];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(TILE_DIM, TILE_DIM);
    dim3 grid((width  + TILE_DIM - 1) / TILE_DIM,
              (height + TILE_DIM - 1) / TILE_DIM);

    for (int i = 0; i < iters; ++i) {
        transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
    }
}
