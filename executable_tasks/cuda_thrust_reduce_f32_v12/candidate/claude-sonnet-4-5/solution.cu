#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARP_SIZE = 32;

__device__ __forceinline__ float warpReduceSum(float val) {
    for (int offset = WARP_SIZE / 2; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* input, float* output, int n) {
    __shared__ float shared[BLOCK_SIZE / WARP_SIZE];
    
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    int gridSize = blockDim.x * gridDim.x;
    
    float sum = 0.0f;
    for (int i = gid; i < n; i += gridSize) {
        sum += input[i];
    }
    
    sum = warpReduceSum(sum);
    
    int lane = tid % WARP_SIZE;
    int wid = tid / WARP_SIZE;
    
    if (lane == 0) {
        shared[wid] = sum;
    }
    __syncthreads();
    
    if (wid == 0) {
        sum = (tid < blockDim.x / WARP_SIZE) ? shared[tid] : 0.0f;
        sum = warpReduceSum(sum);
        if (tid == 0) {
            output[blockIdx.x] = sum;
        }
    }
}

__global__ void final_reduce_kernel(const float* input, float* output, int n) {
    __shared__ float shared[BLOCK_SIZE / WARP_SIZE];
    
    int tid = threadIdx.x;
    
    float sum = 0.0f;
    for (int i = tid; i < n; i += blockDim.x) {
        sum += input[i];
    }
    
    sum = warpReduceSum(sum);
    
    int lane = tid % WARP_SIZE;
    int wid = tid / WARP_SIZE;
    
    if (lane == 0) {
        shared[wid] = sum;
    }
    __syncthreads();
    
    if (wid == 0) {
        sum = (tid < blockDim.x / WARP_SIZE) ? shared[tid] : 0.0f;
        sum = warpReduceSum(sum);
        if (tid == 0) {
            *output = sum;
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    int numBlocks = min((n + BLOCK_SIZE - 1) / BLOCK_SIZE, 1024);
    
    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));
    
    float actual = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<numBlocks, BLOCK_SIZE>>>(input, d_partial, n);
        final_reduce_kernel<<<1, BLOCK_SIZE>>>(d_partial, d_out, numBlocks);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    
    cudaFree(d_partial);
    cudaFree(d_out);
    
    return actual;
}