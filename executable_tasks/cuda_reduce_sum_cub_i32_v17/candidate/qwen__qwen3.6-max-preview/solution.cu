#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void __launch_bounds__(256) reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }

    int lane = threadIdx.x & 31;
    int warpId = threadIdx.x >> 5;

    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    __shared__ int warp_sums[32];
    if (lane == 0) {
        warp_sums[warpId] = sum;
    }
    __syncthreads();

    if (warpId == 0) {
        int val = (lane < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
        #pragma unroll
        for (int offset = 16; offset > 0; offset >>= 1) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (lane == 0) {
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
