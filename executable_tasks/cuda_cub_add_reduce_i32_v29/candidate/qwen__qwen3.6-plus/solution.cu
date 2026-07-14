#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[32];
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int num_threads = blockDim.x;
    int num_blocks = gridDim.x;
    
    int sum = 0;
    for (int i = bid * num_threads + tid; i < n; i += num_blocks * num_threads) {
        sum += input[i];
    }
    
    #pragma unroll
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    if (tid % 32 == 0) {
        sdata[tid / 32] = sum;
    }
    __syncthreads();
    
    if (tid == 0) {
        int block_sum = 0;
        int num_warps = (num_threads + 31) / 32;
        for (int i = 0; i < num_warps; ++i) {
            block_sum += sdata[i];
        }
        atomicAdd(output, block_sum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    const int grid_size = 1024;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid_size, block_size>>>(input, output, n);
    }
}