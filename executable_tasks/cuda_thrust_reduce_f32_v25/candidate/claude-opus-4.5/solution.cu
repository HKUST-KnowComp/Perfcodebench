#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARP_SIZE = 32;

__device__ __forceinline__ float warp_reduce_sum(float val) {
    #pragma unroll
    for (int offset = WARP_SIZE / 2; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float shared[BLOCK_SIZE / WARP_SIZE];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * BLOCK_SIZE + tid;
    int stride = gridDim.x * BLOCK_SIZE;
    
    float sum = 0.0f;
    
    // Grid-stride loop with manual unrolling for better ILP
    while (idx + 3 * stride < n) {
        sum += input[idx];
        sum += input[idx + stride];
        sum += input[idx + 2 * stride];
        sum += input[idx + 3 * stride];
        idx += 4 * stride;
    }
    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }
    
    // Warp-level reduction
    sum = warp_reduce_sum(sum);
    
    int lane = tid % WARP_SIZE;
    int warp_id = tid / WARP_SIZE;
    
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    if (warp_id == 0) {
        sum = (tid < (BLOCK_SIZE / WARP_SIZE)) ? shared[tid] : 0.0f;
        sum = warp_reduce_sum(sum);
        if (tid == 0) {
            output[blockIdx.x] = sum;
        }
    }
}

__global__ void reduce_final_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float shared[BLOCK_SIZE / WARP_SIZE];
    
    int tid = threadIdx.x;
    float sum = 0.0f;
    
    for (int i = tid; i < n; i += BLOCK_SIZE) {
        sum += input[i];
    }
    
    sum = warp_reduce_sum(sum);
    
    int lane = tid % WARP_SIZE;
    int warp_id = tid / WARP_SIZE;
    
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    if (warp_id == 0) {
        sum = (tid < (BLOCK_SIZE / WARP_SIZE)) ? shared[tid] : 0.0f;
        sum = warp_reduce_sum(sum);
        if (tid == 0) {
            *output = sum;
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    
    // Calculate grid size - use enough blocks to saturate GPU but not too many
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    num_blocks = min(num_blocks, 1024);  // Cap at 1024 blocks
    
    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, num_blocks * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));
    
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<num_blocks, BLOCK_SIZE>>>(input, d_partial, n);
        reduce_final_kernel<<<1, BLOCK_SIZE>>>(d_partial, d_out, num_blocks);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    
    cudaFree(d_partial);
    cudaFree(d_out);
    
    return result;
}