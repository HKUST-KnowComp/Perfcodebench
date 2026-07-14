#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ float warp_reduce_sum(float val) {
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float shared[32];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * 4 + tid;
    int stride = blockDim.x * gridDim.x * 4;
    
    float sum = 0.0f;
    
    // Grid-stride loop with 4x unrolling
    while (idx + 3 * blockDim.x < n) {
        sum += input[idx];
        sum += input[idx + blockDim.x];
        sum += input[idx + 2 * blockDim.x];
        sum += input[idx + 3 * blockDim.x];
        idx += stride;
    }
    // Handle remaining elements
    int base = blockIdx.x * blockDim.x * 4 + tid;
    base = idx - stride + stride; // reset to where we left off
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    
    // Warp-level reduction
    int lane = tid & 31;
    int wid = tid >> 5;
    
    sum = warp_reduce_sum(sum);
    
    if (lane == 0) {
        shared[wid] = sum;
    }
    __syncthreads();
    
    // Final reduction in first warp
    if (wid == 0) {
        sum = (tid < (blockDim.x >> 5)) ? shared[lane] : 0.0f;
        sum = warp_reduce_sum(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

__global__ void zero_output(float* output) {
    *output = 0.0f;
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    const int blockSize = 256;
    int numBlocks = (n + blockSize * 4 - 1) / (blockSize * 4);
    if (numBlocks > 1024) numBlocks = 1024;
    if (numBlocks < 1) numBlocks = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_output<<<1, 1>>>(d_out);
        reduce_kernel<<<numBlocks, blockSize>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return result;
}