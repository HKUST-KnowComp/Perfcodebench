#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ float warp_reduce_sum(float val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void fast_reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float shared[8];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * 4 + tid;
    int stride = blockDim.x * gridDim.x * 4;
    
    float sum = 0.0f;
    
    // Grid-stride loop with 4x unrolling for better memory throughput
    while (idx + 3 * blockDim.x < n) {
        sum += input[idx];
        sum += input[idx + blockDim.x];
        sum += input[idx + 2 * blockDim.x];
        sum += input[idx + 3 * blockDim.x];
        idx += stride;
    }
    // Handle remaining elements
    int base = blockIdx.x * blockDim.x * 4 + tid;
    base = (idx - base) / stride * stride + base; // recalculate position
    for (int i = idx; i < n; i += blockDim.x) {
        if (i < n) sum += input[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce_sum(sum);
    
    int warp_id = tid >> 5;
    int lane_id = tid & 31;
    
    if (lane_id == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    if (warp_id == 0) {
        sum = (lane_id < 8) ? shared[lane_id] : 0.0f;
        sum = warp_reduce_sum(sum);
        
        if (lane_id == 0) {
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
    
    const int block_size = 256;
    int num_blocks = (n + block_size * 4 - 1) / (block_size * 4);
    num_blocks = min(num_blocks, 1024);
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_output<<<1, 1>>>(d_out);
        fast_reduce_kernel<<<num_blocks, block_size>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}