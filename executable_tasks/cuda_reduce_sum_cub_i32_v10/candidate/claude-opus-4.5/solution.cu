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
    int sum = 0;
    
    // Vectorized loads: process 4 ints at a time
    int idx4 = (blockIdx.x * blockDim.x + threadIdx.x) * 4;
    int stride4 = blockDim.x * gridDim.x * 4;
    int n4 = (n / 4) * 4;
    
    const int4* input4 = reinterpret_cast<const int4*>(input);
    int n4_vec = n4 / 4;
    
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n4_vec; i += blockDim.x * gridDim.x) {
        int4 v = input4[i];
        sum += v.x + v.y + v.z + v.w;
    }
    
    // Handle remaining elements
    int base = n4 + blockIdx.x * blockDim.x + threadIdx.x;
    if (base < n) {
        sum += input[base];
    }
    
    // Warp-level reduction
    int lane = threadIdx.x & 31;
    int warp_id = threadIdx.x >> 5;
    
    sum = warp_reduce_sum(sum);
    
    // Shared memory for inter-warp reduction
    __shared__ int warp_sums[32];
    
    if (lane == 0) {
        warp_sums[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    if (warp_id == 0) {
        int num_warps = (blockDim.x + 31) >> 5;
        sum = (lane < num_warps) ? warp_sums[lane] : 0;
        sum = warp_reduce_sum(sum);
        
        if (lane == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemsetAsync(output, 0, sizeof(int));
        }
        return;
    }
    
    constexpr int kBlock = 256;
    // Use enough blocks to saturate the GPU but not too many to cause excessive atomics
    int num_blocks = min(1024, (n + kBlock * 4 - 1) / (kBlock * 4));
    num_blocks = max(num_blocks, 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        reduce_kernel<<<num_blocks, kBlock>>>(input, output, n);
    }
}