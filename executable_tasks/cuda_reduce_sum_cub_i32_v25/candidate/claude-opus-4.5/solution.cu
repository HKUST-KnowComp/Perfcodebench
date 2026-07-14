#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle
__device__ __forceinline__ int warp_reduce(int val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

// Block-level reduction: each thread processes multiple elements via grid-stride loop
// Then reduces within block using shared memory and warp shuffles
__global__ void block_reduce_kernel(const int* __restrict__ input, int* __restrict__ partial, int n) {
    __shared__ int sdata[32]; // One slot per warp
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;
    int stride = blockDim.x * gridDim.x;
    
    // Grid-stride loop accumulation
    int sum = 0;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    // First thread of each warp writes to shared memory
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    int num_warps = blockDim.x >> 5;
    if (warp_id == 0) {
        sum = (lane < num_warps) ? sdata[lane] : 0;
        sum = warp_reduce(sum);
        if (lane == 0) {
            partial[blockIdx.x] = sum;
        }
    }
}

// Final reduction kernel: reduces partial sums to single output
__global__ void final_reduce_kernel(const int* __restrict__ partial, int* __restrict__ output, int num_blocks) {
    __shared__ int sdata[32];
    
    int tid = threadIdx.x;
    int sum = 0;
    
    // Each thread accumulates multiple partial sums
    for (int i = tid; i < num_blocks; i += blockDim.x) {
        sum += partial[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce(sum);
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    int num_warps = blockDim.x >> 5;
    if (warp_id == 0) {
        sum = (lane < num_warps) ? sdata[lane] : 0;
        sum = warp_reduce(sum);
        if (lane == 0) {
            *output = sum;
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        cudaMemset(output, 0, sizeof(int));
        return;
    }
    
    constexpr int kBlock = 256;
    // Use enough blocks to saturate GPU but not too many to make final reduction expensive
    int num_blocks = min(1024, (n + kBlock - 1) / kBlock);
    num_blocks = max(1, num_blocks);
    
    // Allocate persistent partial sums buffer
    int* partial = nullptr;
    cudaMalloc(&partial, num_blocks * sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        // First pass: block-level reduction
        block_reduce_kernel<<<num_blocks, kBlock>>>(input, partial, n);
        // Second pass: reduce partial sums to final output
        final_reduce_kernel<<<1, kBlock>>>(partial, output, num_blocks);
    }
    
    cudaFree(partial);
}