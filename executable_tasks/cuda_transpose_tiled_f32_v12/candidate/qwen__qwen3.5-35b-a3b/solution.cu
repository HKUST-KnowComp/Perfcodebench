#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 16;

__global__ void transpose_kernel(const float* input, float* output, int width, int height) {
    extern __shared__ float shared_mem[];
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int start_x = blockIdx.x * kTileDim;
    int start_y = blockIdx.y * kTileDim;

    // Load phase
    if (start_y + ty < height && start_x + tx < width) {
        shared_mem[ty * kTileDim + tx] = input[(start_y + ty) * width + (start_x + tx)];
    }
    __syncthreads();

    // Store phase
    if (start_x + tx < width && start_y + ty < height) {
        output[(start_x + tx) * height + (start_y + ty)] = shared_mem[ty * kTileDim + tx];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    for (int iter = 0; iter < iters; ++iter) {
        transpose_kernel<<<grid, block, kTileDim * kTileDim * sizeof(float)>>>(input, output, width, height);
    }
}