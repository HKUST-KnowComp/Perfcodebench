#include "interface.h"
#include <cuda_runtime.h>

namespace {

template <int BLOCK_SIZE>
__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Each thread accumulates a local sum
    long long local_sum = 0;
    
    // Vectorized loads: process 4 ints at a time
    const int4* input4 = reinterpret_cast<const int4*>(input);
    int n4 = n / 4;
    int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    int stride = BLOCK_SIZE * gridDim.x;
    
    // Process vectorized portion
    for (int i = tid; i < n4; i += stride) {
        int4 v = input4[i];
        local_sum += v.x;
        local_sum += v.y;
        local_sum += v.z;
        local_sum += v.w;
    }
    
    // Process remaining elements
    int base = n4 * 4;
    for (int i = base + tid; i < n; i += stride) {
        local_sum += input[i];
    }
    
    // Warp-level reduction using shuffle
    unsigned int mask = 0xffffffff;
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        local_sum += __shfl_down_sync(mask, local_sum, offset);
    }
    
    // First thread in each warp writes to shared memory
    __shared__ long long warp_sums[BLOCK_SIZE / 32];
    int lane = threadIdx.x & 31;
    int warp_id = threadIdx.x >> 5;
    
    if (lane == 0) {
        warp_sums[warp_id] = local_sum;
    }
    __syncthreads();
    
    // First warp reduces all warp sums
    if (warp_id == 0) {
        local_sum = (lane < (BLOCK_SIZE / 32)) ? warp_sums[lane] : 0;
        #pragma unroll
        for (int offset = (BLOCK_SIZE / 64); offset > 0; offset >>= 1) {
            local_sum += __shfl_down_sync(mask, local_sum, offset);
        }
        
        // Thread 0 does atomic add to global output
        if (lane == 0) {
            atomicAdd(output, static_cast<int>(local_sum));
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
    
    constexpr int BLOCK_SIZE = 256;
    // Use enough blocks to saturate the GPU but not too many to cause excessive atomics
    int num_blocks = min(1024, (n + BLOCK_SIZE * 4 - 1) / (BLOCK_SIZE * 4));
    num_blocks = max(num_blocks, 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        reduce_sum_kernel<BLOCK_SIZE><<<num_blocks, BLOCK_SIZE>>>(input, output, n);
    }
}
