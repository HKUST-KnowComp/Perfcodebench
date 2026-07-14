#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 16;

__global__ void transpose_tiled_kernel(const float* input, float* output, int width, int height) {
    extern __shared__ float shared_mem[];
    
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int block_x = blockIdx.x * kTileDim;
    int block_y = blockIdx.y * kTileDim;
    
    // Load tile from global memory to shared memory
    int in_x = block_x + tx;
    int in_y = block_y + ty;
    if (in_x < width && in_y < height) {
        shared_mem[ty * kTileDim + tx] = input[in_y * width + in_x];
    }
    
    __syncthreads();
    
    // Transpose in shared memory and write to global memory
    int out_x = block_y + ty;
    int out_y = block_x + tx;
    if (out_x < height && out_y < width) {
        output[out_x * height + out_y] = shared_mem[tx * kTileDim + ty];
    }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    const dim3 block(kTileDim, kTileDim);
    const dim3 grid((width + kTileDim - 1) / kTileDim, (height + kTileDim - 1) / kTileDim);
    const size_t shared_mem_size = kTileDim * kTileDim * sizeof(float);
    
    for (int iter = 0; iter < iters; ++iter) {
        transpose_tiled_kernel<<<grid, block, shared_mem_size>>>(input, output, width, height);
    }
}