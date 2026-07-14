#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[8];
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warpId = tid >> 5;

    int sum = 0;
    for (int i = blockIdx.x * 256 + tid; i < n; i += gridDim.x * 256) {
        sum += input[i];
    }

    sum += __shfl_down_sync(0xffffffff, sum, 16);
    sum += __shfl_down_sync(0xffffffff, sum, 8);
    sum += __shfl_down_sync(0xffffffff, sum, 4);
    sum += __shfl_down_sync(0xffffffff, sum, 2);
    sum += __shfl_down_sync(0xffffffff, sum, 1);

    if (lane == 0) sdata[warpId] = sum;
    __syncthreads();

    if (warpId == 0) {
        int val = (tid < 8) ? sdata[tid] : 0;
        val += __shfl_down_sync(0xffffffff, val, 4);
        val += __shfl_down_sync(0xffffffff, val, 2);
        val += __shfl_down_sync(0xffffffff, val, 1);
        if (tid == 0) atomicAdd(output, val);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int blocks = (n + 255) / 256;
    if (blocks > 512) blocks = 512;
    if (blocks < 1) blocks = 1;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, 256>>>(input, output, n);
    }
}
