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

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    extern __shared__ float sdata[];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    const int grid_size = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;
    
    float sum = 0.0f;
    
    // Grid-stride loop with manual unrolling for better ILP
    while (idx + 3 * grid_size < n) {
        sum += input[idx];
        sum += input[idx + grid_size];
        sum += input[idx + 2 * grid_size];
        sum += input[idx + 3 * grid_size];
        idx += 4 * grid_size;
    }
    while (idx < n) {
        sum += input[idx];
        idx += grid_size;
    }
    
    // Warp-level reduction
    sum = warp_reduce_sum(sum);
    
    // Store warp results to shared memory
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    const int num_warps = blockDim.x >> 5;
    if (warp_id == 0) {
        sum = (tid < num_warps) ? sdata[tid] : 0.0f;
        sum = warp_reduce_sum(sum);
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
    if (n <= 0) return 0.0f;
    
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    // Tuned parameters
    const int block_size = 256;
    const int num_warps = block_size >> 5;
    const int shared_mem = num_warps * sizeof(float);
    
    // Limit blocks to avoid over-subscription while ensuring enough work per thread
    int num_blocks = (n + block_size - 1) / block_size;
    if (num_blocks > 256) num_blocks = 256;
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_kernel<<<1, 1>>>(d_out);
        reduce_kernel<<<num_blocks, block_size, shared_mem>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}