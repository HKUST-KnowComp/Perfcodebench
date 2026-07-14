#include "interface.h"
#include <cuda_runtime.h>

namespace {
// Each thread handles 8 values -> 8x32 tile processed by 32x8 block
constexpr int BLOCK_X = 32;
constexpr int BLOCK_Y = 8;
constexpr int PAD     = 1;                       // prevent shared-memory bank conflicts

__global__ void kernel(const float* __restrict__ in,
                       float* __restrict__ out,
                       int w, int h)
{
  // each thread handles 8 consecutive rows in the tile
  __shared__ float tile[BLOCK_Y*8][BLOCK_X+PAD];

  int gx = blockIdx.x * BLOCK_X + threadIdx.x;
  int gy = blockIdx.y * BLOCK_Y*8;

  // load from global memory (coalesced) into shared memory transposed position
  #pragma unroll
  for (int i=0;i<8;i++)
  {
    int yy = gy + threadIdx.y + i*BLOCK_Y;
    if (gx<w && yy<h)
      tile[threadIdx.y+i*BLOCK_Y][threadIdx.x] = in[yy*w+gx];
  }
  __syncthread();

  // write back transposed tile (now coalesced again)
  gy = blockIdx.y * BLOCK_Y*8;
  int ny = gy + threadIdx.x;          // new y after transpose
  int nx = blockIdx.x * BLOCK_X + threadIdx.y;
  #pragma unroll
  for (int i=0;i<8;i++)
  {
    if (nx<w && ny+i*BLOCK_X<h)
      out[(ny+i*BLOCK_X)*w+nx] = tile[threadIdx.y+i*BLOCK_X][threadIdx.x];
  }
}

} // anon

void transpose_f32(const float* input, float* output,
                   int width, int height, int iters)
{
  dim3 block(BLOCK_X, BLOCK_Y);
  dim3 grid((width  + BLOCK_X - 1) / BLOCK_X,
            (height + BLOCK_Y*8 - 1) / (BLOCK_Y*8));

  for (int i=0;i<iters;i++)
    kernel<<<grid, block>>>(input, output, width, height);
}