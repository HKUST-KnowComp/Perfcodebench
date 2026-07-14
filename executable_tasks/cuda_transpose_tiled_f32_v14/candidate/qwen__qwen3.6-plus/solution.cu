#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BD = 32;

__global__ void transpose_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[BD][BD + 1];
    
    int col = blockIdx.x * BD + threadIdx.x;
    int row = blockIdx.y * BD + threadIdx.y;
    
    if (col < width && row < height) {
        tile[threadIdx.y][threadIdx.x] = input[row * width + col];
    }
    __syncthreads();
    
    col = blockIdx.y * BD + threadIdx.x;
    row = blockIdx.x * BD + threadIdx.y;
    
    if (col < height && row < width) {
        output[row * height + col] = tile[threadIdx.x][threadIdx.y];
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    dim3 block(BD, BD);
    dim3 grid((width + BD - 1) / BD, (height + BD - 1) / BD);
    
    for (int i = 0; i < iters; ++i) {
        transpose_kernel<<<grid, block>>>(input, output, width, height);
    }
}
