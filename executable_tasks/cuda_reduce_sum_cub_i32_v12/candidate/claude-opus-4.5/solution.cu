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
    __shared__ int shared[32];
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int sum = 0;
    
    // Vectorized load with int4
    int idx4 = (blockIdx.x * blockDim.x + tid) * 4;
    int stride4 = blockDim.x * gridDim.x * 4;
    int n4 = n & ~3;
    
    const int4* input4 = reinterpret_cast<const int4*>(input);
    int num_vec = n4 >> 2;
    
    for (int i = blockIdx.x * blockDim.x + tid; i < num_vec; i += blockDim.x * gridDim.x) {
        int4 v = input4[i];
        sum += v.x + v.y + v.z + v.w;
    }
    
    // Handle tail elements
    int base = n4;
    for (int i = base + blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce_sum(sum);
    
    // First thread of each warp writes to shared memory
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    int num_warps = blockDim.x >> 5;
    if (warp_id == 0) {
        sum = (tid < num_warps) ? shared[tid] : 0;
        sum = warp_reduce_sum(sum);
        
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemsetAsync(output, 0, sizeof(int));
        }
        return;
    }
    
    constexpr int kBlock = 256;
    int num_blocks = (n + kBlock * 4 - 1) / (kBlock * 4);
    num_blocks = min(num_blocks, 1024);
    num_blocks = max(num_blocks, 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        reduce_kernel<<<num_blocks, kBlock>>>(input, output, n);
    }
}