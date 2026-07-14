#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle
__device__ __forceinline__ float warp_reduce(float val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

// Block-level reduction kernel with grid-stride loop
__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    extern __shared__ float sdata[];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int wid = tid >> 5;
    const int grid_size = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;
    
    // Grid-stride loop accumulation
    float sum = 0.0f;
    while (idx < n) {
        sum += input[idx];
        idx += grid_size;
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    // First thread of each warp writes to shared memory
    if (lane == 0) {
        sdata[wid] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    const int num_warps = blockDim.x >> 5;
    if (tid < num_warps) {
        sum = sdata[tid];
    } else {
        sum = 0.0f;
    }
    
    if (wid == 0) {
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
    
    // Tuned parameters
    const int block_size = 256;
    const int num_warps = block_size >> 5;
    const int smem_size = num_warps * sizeof(float);
    
    // Limit blocks to avoid over-subscription while ensuring good occupancy
    int num_blocks = (n + block_size - 1) / block_size;
    if (num_blocks > 256) num_blocks = 256;
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_kernel<<<1, 1>>>(d_out);
        reduce_kernel<<<num_blocks, block_size, smem_size>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}