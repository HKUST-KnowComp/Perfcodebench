#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const float* input, float* output, int n) {
    // Use a grid-stride loop to handle any input size
    float sum = 0.0f;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = tid; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Block-level reduction using shared memory
    static __shared__ float shared_sums[32];
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[wid] = sum;
    }
    __syncthreads();

    // Final reduction in the first warp of the first block
    if (wid == 0 && lane < (blockDim.x + 31) / 32) {
        float block_sum = shared_sums[lane];
        for (int offset = 16; offset > 0; offset /= 2) {
            block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, block_sum);
        }
    }
}

} // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));

    // Tuning parameters for occupancy
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    if (blocksPerGrid > 1024) blocksPerGrid = 1024;

    float final_result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<blocksPerGrid, threadsPerBlock>>>(input, d_out, n);
        cudaDeviceSynchronize();
        cudaMemcpy(&final_result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    }

    cudaFree(d_out);
    return final_result;
}