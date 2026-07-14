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

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int shared[32];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    int sum = 0;
    
    // Vectorized loads using int4 (4 ints at a time)
    const int4* input4 = reinterpret_cast<const int4*>(input);
    const int n4 = n >> 2;
    
    int idx4 = blockIdx.x * blockDim.x + tid;
    const int stride4 = blockDim.x * gridDim.x;
    
    // Process 4 elements at a time
    while (idx4 < n4) {
        int4 v = input4[idx4];
        sum += v.x + v.y + v.z + v.w;
        idx4 += stride4;
    }
    
    // Handle remaining elements
    int idx = (n4 << 2) + (blockIdx.x * blockDim.x + tid);
    const int stride = blockDim.x * gridDim.x;
    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }
    
    // Warp-level reduction
    sum = warp_reduce_sum(sum);
    
    // First thread in each warp writes to shared memory
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    const int num_warps = blockDim.x >> 5;
    if (warp_id == 0) {
        sum = (tid < num_warps) ? shared[tid] : 0;
        sum = warp_reduce_sum(sum);
        
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    constexpr int kBlock = 256;
    const int grid = min(1024, (n + kBlock - 1) / kBlock);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        reduce_sum_kernel<<<grid, kBlock>>>(input, output, n);
    }
}