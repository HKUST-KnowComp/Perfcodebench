#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ float warp_reduce(float val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    extern __shared__ float sdata[];
    
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int bdim = blockDim.x;
    const int grid_size = bdim * gridDim.x;
    
    int idx = bid * bdim + tid;
    
    float sum = 0.0f;
    
    // Grid-stride loop for coalesced access
    while (idx < n) {
        sum += input[idx];
        idx += grid_size;
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    // First thread of each warp writes to shared memory
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    const int num_warps = (bdim + 31) >> 5;
    if (warp_id == 0) {
        sum = (tid < num_warps) ? sdata[tid] : 0.0f;
        sum = warp_reduce(sum);
        if (tid == 0) {
            output[bid] = sum;
        }
    }
}

__global__ void final_reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    extern __shared__ float sdata[];
    
    const int tid = threadIdx.x;
    
    float sum = 0.0f;
    for (int i = tid; i < n; i += blockDim.x) {
        sum += input[i];
    }
    
    sum = warp_reduce(sum);
    
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    const int num_warps = (blockDim.x + 31) >> 5;
    if (warp_id == 0) {
        sum = (tid < num_warps) ? sdata[tid] : 0.0f;
        sum = warp_reduce(sum);
        if (tid == 0) {
            *output = sum;
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    
    const int block_size = 256;
    const int max_blocks = 1024;
    int num_blocks = (n + block_size - 1) / block_size;
    if (num_blocks > max_blocks) num_blocks = max_blocks;
    
    const int num_warps = (block_size + 31) >> 5;
    const size_t shared_size = num_warps * sizeof(float);
    
    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, num_blocks * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));
    
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<num_blocks, block_size, shared_size>>>(input, d_partial, n);
        final_reduce_kernel<<<1, block_size, shared_size>>>(d_partial, d_out, num_blocks);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    
    cudaFree(d_partial);
    cudaFree(d_out);
    
    return result;
}
