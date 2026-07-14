#include "interface.h"
#include <cuda_runtime.h>
#include <cstdint>

namespace {

constexpr int TILE = 32;                 // tile width/height in elements
constexpr int BLOCK_ROWS = 8;              // rows each thread handles (TILE/BLOCK_ROWS==4)

template<int STRIDE>
__device__ inline void transpose_naive_vec4(const float4* __restrict__ in,
                                            float4*       __restrict__ out,
                                            int width, int height)
{
    int x0 = blockIdx.x * TILE + threadIdx.x;
    int y0 = blockIdx.y * TILE + threadIdx.y;

    #pragma unroll
    for (int j = 0; j < STRIDE; ++j){
        int y = y0 + j * BLOCK_ROWS;
        if (x0 < width && y < height){
            out[x0 * height + y] = in[y * width + x0];
        }
    }
}

__global__ void transpose_vec4(const float* __restrict__ input,
                               float*       __restrict__ output,
                               int width, int height)
{
    const float4* in4  = reinterpret_cast<const float4*>(input);
    float4*       out4 = reinterpret_cast<float4*>(output);

    int x4 = blockIdx.x * TILE + threadIdx.x;
    int y0 = blockIdx.y * TILE + threadIdx.y;

    #pragma unroll
    for (int j = 0; j < 4; ++j){
        int y4 = y0 + j * BLOCK_ROWS;
        if (x4 < width/4 && y4 < height/4){
            out4[x4 * (height/4) + y4] = in4[y4 * (width/4) + x4];
        }
    }
}

__global__ void transpose_scalar_tail(const float* __restrict__ input,
                                      float*       __restrict__ output,
                                      int width, int height,
                                      int vec_rows, int vec_cols)
{
    int x = blockIdx.x * TILE + threadIdx.x;
    int y0 = blockIdx.y * TILE + threadIdx.y;
    if (x < vec_cols) return;      // already handled by vector path
    #pragma unroll
    for (int j = 0; j < 4; ++j){
        int y = y0 + j * BLOCK_ROWS;
        if (x < width && y < height){
            output[x * height + y] = input[y * width + x];
        }
    }
}

} // anonymous namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters)
{
    dim3 block(TILE, BLOCK_ROWS, 1);
    int vec_cols = (width  / 4) * 4;
    int vec_rows = (height / 4) * 4;

    dim3 grid_vec ((vec_cols  + TILE - 1) / TILE,
                   (vec_rows  + TILE - 1) / TILE);
    dim3 grid_tail((width     + TILE - 1) / TILE,
                   (height    + TILE - 1) / TILE);

    for (int it = 0; it < iters; ++it){
        if (width >= 4 && height >= 4){
            transpose_vec4<<<grid_vec, block>>>(input, output, vec_cols, vec_rows);
        }
        if (width > vec_cols || height > vec_rows){
            transpose_scalar_tail<<<grid_tail, block>>>(input, output,
                                                       width, height,
                                                       vec_rows, vec_cols);
        }
    }
}