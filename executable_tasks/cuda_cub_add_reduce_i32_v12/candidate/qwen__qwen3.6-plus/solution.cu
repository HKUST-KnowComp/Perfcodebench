#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void fast_reduce_kernel(const int* input, int* output, int n) {
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int bdim = blockDim.x;
    int gridSize = bdim * gridDim.x;
    
    int sum = 0;
    for (int i = bid * bdim + tid; i < n; i += gridSize) {
        sum += input[i];
    }
    
    // Warp-level reduction using shuffle
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    // Store warp leaders' sums in shared memory
    __shared__ int warp_sums[8];
    if (tid % 32 == 0) {
        warp_sums[tid / 32] = sum;
    }
    __syncthreads();
    
    // Final reduction in the first warp
    if (tid < 32) {
        int val = (tid < 8) ? warp_sums[tid] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        fast_reduce_kernel<<<1024, 256>>>(input, output, n);
    }
}
