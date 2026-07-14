#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[32];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    int sum = 0;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    
    #pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    if (tid % warpSize == 0) {
        sdata[tid / warpSize] = sum;
    }
    __syncthreads();
    
    if (tid < warpSize) {
        int val = (tid < blockDim.x / warpSize) ? sdata[tid] : 0;
        #pragma unroll
        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    if (grid < 1) grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
