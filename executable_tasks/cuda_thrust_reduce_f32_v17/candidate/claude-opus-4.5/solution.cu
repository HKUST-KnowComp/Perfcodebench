#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void fast_reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float sdata[8];  // One float per warp (256 threads / 32 = 8 warps)
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warpId = tid >> 5;
    const int gridSize = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;
    
    // Grid-stride loop accumulation
    float sum = 0.0f;
    while (idx < n) {
        sum += input[idx];
        idx += gridSize;
    }
    
    // Warp-level reduction using shuffle
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    // First lane of each warp writes to shared memory
    if (lane == 0) {
        sdata[warpId] = sum;
    }
    __syncthreads();
    
    // First warp reduces the partial sums from all warps
    if (warpId == 0) {
        sum = (tid < 8) ? sdata[tid] : 0.0f;
        #pragma unroll
        for (int offset = 4; offset > 0; offset >>= 1) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    const int blockSize = 256;
    // Use enough blocks to saturate the GPU but not too many to cause excessive atomics
    int numBlocks = (n + blockSize - 1) / blockSize;
    if (numBlocks > 256) numBlocks = 256;
    if (numBlocks < 1) numBlocks = 1;
    
    float result = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(d_out, 0, sizeof(float));
        fast_reduce_kernel<<<numBlocks, blockSize>>>(input, d_out, n);
    }
    
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}