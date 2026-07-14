#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    __shared__ int warp_sums[32];
    int lane = threadIdx.x & 31;
    int wid = threadIdx.x >> 5;

    if (lane == 0) warp_sums[wid] = sum;
    __syncthreads();

    if (wid == 0) {
        int wsum = (threadIdx.x < (blockDim.x >> 5)) ? warp_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            wsum += __shfl_down_sync(0xffffffff, wsum, offset);
        }
        if (lane == 0) atomicAdd(output, wsum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    if (blocks < 1) blocks = 1;
    if (blocks > 1024) blocks = 1024;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads>>>(input, output, n);
    }
}