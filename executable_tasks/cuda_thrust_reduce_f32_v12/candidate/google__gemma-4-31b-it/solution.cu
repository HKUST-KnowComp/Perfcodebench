#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_kernel(const float* __restrict__ input, float* output, int n) {
    // Use a shared memory variable for the final accumulation of this block
    static __shared__ float block_sum;
    if (threadIdx.x == 0) block_sum = 0.0f;
    __syncthreads();

    float sum = 0.0f;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    // Grid-stride loop to handle n > total threads
    for (int i = tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    // Warp-level reduction using shuffle
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Thread 0 of each warp adds to block_sum
    if ((threadIdx.x & 31) == 0) {
        atomicAdd(&block_sum, sum);
    }
    __syncthreads();

    // Thread 0 of block adds to global output
    if (threadIdx.x == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));

    // Tuning parameters
    const int blockSize = 256;
    const int gridSize = (n + blockSize - 1) / blockSize;
    // Cap gridSize to avoid excessive overhead for very large n
    const int cappedGridSize = (gridSize > 1024) ? 1024 : gridSize;

    float h_result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<cappedGridSize, blockSize>>>(input, d_out, n);
        cudaDeviceSynchronize();
    }

    cudaMemcpy(&h_result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return h_result;
}