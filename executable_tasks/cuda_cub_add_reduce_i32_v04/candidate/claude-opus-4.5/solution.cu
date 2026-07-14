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

__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Each thread accumulates multiple elements
    int sum = 0;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    // Grid-stride loop with vectorized loads when possible
    int idx = tid * 4;
    int stride4 = stride * 4;
    
    // Process 4 elements at a time using int4
    while (idx + 3 < n) {
        int4 vals = *reinterpret_cast<const int4*>(input + idx);
        sum += vals.x + vals.y + vals.z + vals.w;
        idx += stride4;
    }
    
    // Handle remaining elements
    int remaining_start = (n / 4) * 4;
    for (int i = tid + remaining_start; i < n; i += stride) {
        sum += input[i];
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
        int num_warps = blockDim.x >> 5;
        sum = (lane < num_warps) ? warp_sums[lane] : 0;
        sum = warp_reduce_sum(sum);
        
        if (lane == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemsetAsync(output, 0, sizeof(int));
        }
        return;
    }
    
    // Optimal configuration: 256 threads per block
    const int block_size = 256;
    // Use enough blocks to saturate the GPU but not too many to cause excessive atomics
    // Each thread processes multiple elements via grid-stride loop
    int num_blocks = min(1024, (n + block_size * 4 - 1) / (block_size * 4));
    num_blocks = max(num_blocks, 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        optimized_reduce_kernel<<<num_blocks, block_size>>>(input, output, n);
    }
}