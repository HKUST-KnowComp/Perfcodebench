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
    __shared__ float shared[32]; // One element per warp
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    // Grid-stride loop accumulation
    float sum = 0.0f;
    int idx = blockIdx.x * blockDim.x + tid;
    int stride = blockDim.x * gridDim.x;
    
    // Process 4 elements per iteration when possible
    int idx4 = idx * 4;
    int stride4 = stride * 4;
    int n4 = (n / 4) * 4;
    
    while (idx4 + 3 < n4) {
        float4 v = *reinterpret_cast<const float4*>(input + idx4);
        sum += v.x + v.y + v.z + v.w;
        idx4 += stride4;
    }
    
    // Handle remaining elements
    int remaining_start = n4;
    idx = remaining_start + (blockIdx.x * blockDim.x + tid) % (n - remaining_start + 1);
    if (idx < n && remaining_start < n) {
        for (int i = remaining_start + ((blockIdx.x * blockDim.x + tid) % stride); i < n; i += stride) {
            sum += input[i];
        }
    }
    // Simpler tail handling
    idx = blockIdx.x * blockDim.x + tid;
    for (int i = n4 + idx; i < n; i += stride) {
        sum += input[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    // First lane of each warp writes to shared memory
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp results
    int num_warps = blockDim.x >> 5;
    if (warp_id == 0) {
        sum = (tid < num_warps) ? shared[tid] : 0.0f;
        sum = warp_reduce(sum);
        
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

// Simple kernel to zero output
__global__ void zero_kernel(float* output) {
    *output = 0.0f;
}

} // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    // Choose block and grid size
    const int block_size = 256;
    int num_blocks = (n + block_size * 4 - 1) / (block_size * 4);
    num_blocks = min(num_blocks, 1024); // Cap grid size
    num_blocks = max(num_blocks, 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_kernel<<<1, 1>>>(d_out);
        reduce_kernel<<<num_blocks, block_size>>>(input, d_out, n);
    }
    
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}