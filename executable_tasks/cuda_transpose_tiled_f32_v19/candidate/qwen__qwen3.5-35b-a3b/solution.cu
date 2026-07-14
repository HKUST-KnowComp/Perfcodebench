#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    __shared__ float tile[32][32];
    int x = blockIdx.x * kTileDim;
    int y = blockIdx.y * kTileDim;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Load into shared memory
    if (x + tx < width && y + ty < height) {
        tile[ty][tx] = input[(y + ty) * width + (x + tx)];
    }
    __syncthreads();

    // Transpose and write to output
    if (x + tx < width && y + ty < height) {
        output[(x + tx) * height + (y + ty)] = tile[ty][tx];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}