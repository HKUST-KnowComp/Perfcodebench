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
    __shared__ float shared[8];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    const int grid_size = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;
    
    float sum = 0.0f;
    
    // Vectorized loads with float4
    const int n4 = n >> 2;
    const float4* input4 = reinterpret_cast<const float4*>(input);
    int idx4 = idx;
    
    while (idx4 < n4) {
        float4 v = input4[idx4];
        sum += v.x + v.y + v.z + v.w;
        idx4 += grid_size;
    }
    
    // Handle remaining elements
    int remaining_start = n4 << 2;
    int remaining_idx = remaining_start + (idx & (grid_size - 1));
    if (remaining_idx < n && idx < grid_size) {
        for (int i = remaining_idx; i < n; i += grid_size) {
            sum += input[i];
        }
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    // Store warp results to shared memory
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    if (warp_id == 0) {
        sum = (tid < 8) ? shared[tid] : 0.0f;
        sum = warp_reduce(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    const int block_size = 256;
    int num_blocks = (n + block_size * 4 - 1) / (block_size * 4);
    num_blocks = min(num_blocks, 256);
    
    float result = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(d_out, 0, sizeof(float));
        reduce_kernel<<<num_blocks, block_size>>>(input, d_out, n);
    }
    
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}
