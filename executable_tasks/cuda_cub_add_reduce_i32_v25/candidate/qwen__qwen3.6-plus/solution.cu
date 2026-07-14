#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int warp_sums[8];
    unsigned int tid = threadIdx.x;
    unsigned int warpId = tid >> 5;
    unsigned int laneId = tid & 31;
    unsigned int i = blockIdx.x * 256 + threadIdx.x;
    unsigned int gridSize = 256 * gridDim.x;

    int sum = 0;
    while (i < n) {
        sum += input[i];
        i += gridSize;
    }

    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    if (laneId == 0) warp_sums[warpId] = sum;
    __syncthreads();

    if (warpId == 0) {
        int val = (tid < 8) ? warp_sums[tid] : 0;
        for (int offset = 4; offset > 0; offset >>= 1) {
            val += __shfl_down_sync(0xffffffff, val, offset);
        }
        if (tid == 0) atomicAdd(output, val);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<1024, 256>>>(input, output, n);
    }
}
