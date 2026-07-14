#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle
__device__ __forceinline__ int warp_reduce_sum(int val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

// Block-level reduction kernel - each block produces one partial sum
__global__ void block_reduce_kernel(const int* __restrict__ input, int* __restrict__ partials, int n) {
    __shared__ int sdata[32]; // One element per warp
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * 8 + tid; // Each thread processes 8 elements
    int stride = blockDim.x;
    int grid_stride = gridDim.x * blockDim.x * 8;
    
    // Each thread accumulates multiple elements
    int sum = 0;
    
    // Process 8 elements per thread per iteration
    while (idx + 7 * stride < n) {
        sum += input[idx];
        sum += input[idx + stride];
        sum += input[idx + 2 * stride];
        sum += input[idx + 3 * stride];
        sum += input[idx + 4 * stride];
        sum += input[idx + 5 * stride];
        sum += input[idx + 6 * stride];
        sum += input[idx + 7 * stride];
        idx += grid_stride;
    }
    
    // Handle remaining elements
    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }
    
    // Warp-level reduction
    int warp_id = tid / 32;
    int lane_id = tid % 32;
    
    sum = warp_reduce_sum(sum);
    
    // First thread of each warp writes to shared memory
    if (lane_id == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    if (warp_id == 0) {
        sum = (tid < (blockDim.x / 32)) ? sdata[lane_id] : 0;
        sum = warp_reduce_sum(sum);
        
        if (tid == 0) {
            partials[blockIdx.x] = sum;
        }
    }
}

// Final reduction kernel - reduces partial sums to single value
__global__ void final_reduce_kernel(const int* __restrict__ partials, int* __restrict__ output, int num_partials) {
    __shared__ int sdata[32];
    
    int tid = threadIdx.x;
    int sum = 0;
    
    // Each thread sums multiple partials
    for (int i = tid; i < num_partials; i += blockDim.x) {
        sum += partials[i];
    }
    
    // Warp-level reduction
    int warp_id = tid / 32;
    int lane_id = tid % 32;
    
    sum = warp_reduce_sum(sum);
    
    if (lane_id == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    if (warp_id == 0) {
        sum = (tid < (blockDim.x / 32)) ? sdata[lane_id] : 0;
        sum = warp_reduce_sum(sum);
        
        if (tid == 0) {
            *output = sum;
        }
    }
}

// Persistent buffer for partial sums
static int* d_partials = nullptr;
static int partials_capacity = 0;

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) {
        cudaMemset(output, 0, sizeof(int));
        return;
    }
    
    constexpr int kBlock = 256;
    constexpr int kElementsPerThread = 8;
    constexpr int kElementsPerBlock = kBlock * kElementsPerThread;
    
    // Calculate grid size - limit to reasonable number of blocks
    int num_blocks = (n + kElementsPerBlock - 1) / kElementsPerBlock;
    num_blocks = min(num_blocks, 1024);
    
    // Allocate/reallocate partial sums buffer if needed
    if (num_blocks > partials_capacity) {
        if (d_partials) cudaFree(d_partials);
        cudaMalloc(&d_partials, num_blocks * sizeof(int));
        partials_capacity = num_blocks;
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        // First pass: block-level reduction
        block_reduce_kernel<<<num_blocks, kBlock>>>(input, d_partials, n);
        
        // Second pass: reduce partial sums
        final_reduce_kernel<<<1, kBlock>>>(d_partials, output, num_blocks);
    }
}