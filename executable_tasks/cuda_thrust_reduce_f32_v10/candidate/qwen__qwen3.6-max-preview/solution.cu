#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    float sum = 0.0f;
    // Grid-stride loop for coalesced memory access and arbitrary n
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    // Intra-warp reduction using shuffle instructions
    for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Shared memory for inter-warp reduction
    __shared__ float sdata[32];
    int lane = threadIdx.x % warpSize;
    int wid = threadIdx.x / warpSize;

    if (lane == 0) sdata[wid] = sum;
    __syncthreads();

    // First warp reduces the partial sums from all warps in the block
    if (wid == 0) {
        sum = (threadIdx.x < blockDim.x / warpSize) ? sdata[lane] : 0.0f;
        for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        if (threadIdx.x == 0) {
            atomicAdd(output, sum);
        }
    }
}
} // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    int blockSize = 256;
    int numBlocks = (n + blockSize - 1) / blockSize;
    if (numBlocks > 1024) numBlocks = 1024; // Cap to limit atomic contention

    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<numBlocks, blockSize>>>(input, d_out, n);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}