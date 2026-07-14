#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void fast_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[32];
    int tid = threadIdx.x;
    int warp_id = tid / 32;
    int lane_id = tid % 32;
    
    int sum = 0;
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    if (lane_id == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    if (tid == 0) {
        int block_sum = 0;
        int num_warps = blockDim.x / 32;
        for (int i = 0; i < num_warps; ++i) {
            block_sum += sdata[i];
        }
        atomicAdd(output, block_sum);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    if (grid < 1) grid = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        fast_reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}