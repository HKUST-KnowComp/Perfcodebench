#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use vectorized loads for better memory throughput
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    // Grid-stride loop with vectorized access
    const int4* input4 = reinterpret_cast<const int4*>(input);
    const int n4 = n >> 2;
    const int remainder_start = n4 << 2;
    
    long long local_sum = 0;
    
    // Process 4 elements at a time
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n4; idx += blockDim.x * gridDim.x) {
        int4 val = input4[idx];
        local_sum += val.x;
        local_sum += val.y;
        local_sum += val.z;
        local_sum += val.w;
    }
    
    // Handle remainder elements
    for (int idx = remainder_start + blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        local_sum += input[idx];
    }
    
    // Warp-level reduction using shuffle
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        local_sum += __shfl_down_sync(0xffffffff, local_sum, offset);
    }
    
    // Shared memory for inter-warp reduction
    __shared__ long long warp_sums[32];
    
    if (lane == 0) {
        warp_sums[warp_id] = local_sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    if (warp_id == 0) {
        const int num_warps = blockDim.x >> 5;
        local_sum = (tid < num_warps) ? warp_sums[tid] : 0LL;
        
        #pragma unroll
        for (int offset = 16; offset > 0; offset >>= 1) {
            local_sum += __shfl_down_sync(0xffffffff, local_sum, offset);
        }
        
        if (tid == 0) {
            atomicAdd(reinterpret_cast<unsigned long long*>(output), 
                      static_cast<unsigned long long>(local_sum));
        }
    }
}

__global__ void finalize_kernel(int* output) {
    long long val = *reinterpret_cast<long long*>(output);
    output[0] = static_cast<int>(val);
    output[1] = 0;
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = min(1024, (n + kBlock * 4 - 1) / (kBlock * 4));
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(long long));
        reduce_sum_kernel<<<grid, kBlock>>>(input, output, n);
        finalize_kernel<<<1, 1>>>(output);
    }
}