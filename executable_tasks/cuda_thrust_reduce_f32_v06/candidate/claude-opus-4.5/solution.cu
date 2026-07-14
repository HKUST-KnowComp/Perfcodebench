#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARP_SIZE = 32;

__device__ __forceinline__ float warp_reduce(float val) {
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
    sum = warp_reduce(sum);
    
    int lane = tid % WARP_SIZE;
    int warp_id = tid / WARP_SIZE;
    
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    if (warp_id == 0) {
        sum = (tid < BLOCK_SIZE / WARP_SIZE) ? shared[tid] : 0.0f;
        sum = warp_reduce(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

__global__ void zero_kernel(float* output) {
    *output = 0.0f;
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    // Calculate optimal grid size
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // Cap blocks to avoid too many atomics, but ensure enough parallelism
    int max_blocks = 1024;
    if (num_blocks > max_blocks) num_blocks = max_blocks;
    if (num_blocks < 1) num_blocks = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_kernel<<<1, 1>>>(d_out);
        reduce_kernel<<<num_blocks, BLOCK_SIZE>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}