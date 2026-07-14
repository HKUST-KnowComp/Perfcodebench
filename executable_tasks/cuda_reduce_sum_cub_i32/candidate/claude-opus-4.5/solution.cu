#include "interface.h"

#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warp_reduce_sum(int val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    constexpr int BLOCK_SIZE = 256;
    constexpr int WARPS_PER_BLOCK = BLOCK_SIZE / 32;
    
    __shared__ int warp_sums[WARPS_PER_BLOCK];
    
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    // Grid-stride loop accumulation
    int sum = 0;
    for (int i = gid; i < n; i += stride) {
        sum += input[i];
    }
    
    // Warp-level reduction
    int warp_sum = warp_reduce_sum(sum);
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    // First thread of each warp writes to shared memory
    if (lane == 0) {
        warp_sums[warp_id] = warp_sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    if (warp_id == 0) {
        int val = (tid < WARPS_PER_BLOCK) ? warp_sums[tid] : 0;
        val = warp_reduce_sum(val);
        
        // Single atomic per block
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    // Use enough blocks to saturate GPU but not too many to cause excessive atomics
    int num_blocks = min(1024, (n + kBlock - 1) / kBlock);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        reduce_kernel<<<num_blocks, kBlock>>>(input, output, n);
    }
}