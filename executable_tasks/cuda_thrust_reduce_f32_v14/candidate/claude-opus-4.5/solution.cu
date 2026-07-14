#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARPS_PER_BLOCK = BLOCK_SIZE / 32;

__device__ __forceinline__ float warp_reduce(float val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float shared[WARPS_PER_BLOCK];
    
    int tid = threadIdx.x;
    int global_tid = blockIdx.x * BLOCK_SIZE + tid;
    int grid_size = gridDim.x * BLOCK_SIZE;
    
    float sum = 0.0f;
    
    // Grid-stride loop with vectorized loads
    int vec_n = n / 4;
    const float4* input4 = reinterpret_cast<const float4*>(input);
    
    for (int i = global_tid; i < vec_n; i += grid_size) {
        float4 v = input4[i];
        sum += v.x + v.y + v.z + v.w;
    }
    
    // Handle remaining elements
    int base = vec_n * 4;
    for (int i = base + global_tid; i < n; i += grid_size) {
        sum += input[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    if (warp_id == 0) {
        sum = (tid < WARPS_PER_BLOCK) ? shared[tid] : 0.0f;
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
    
    // Calculate grid size - use enough blocks to saturate GPU but not too many
    int num_blocks = (n + BLOCK_SIZE * 4 - 1) / (BLOCK_SIZE * 4);
    num_blocks = min(num_blocks, 1024);
    num_blocks = max(num_blocks, 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_kernel<<<1, 1>>>(d_out);
        reduce_kernel<<<num_blocks, BLOCK_SIZE>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}