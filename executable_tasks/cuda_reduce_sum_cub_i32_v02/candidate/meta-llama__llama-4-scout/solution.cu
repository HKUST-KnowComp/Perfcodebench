#include "interface.h"
#include <cub/cub.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;
    __shared__ int sdata[256];
    sdata[tid] = 0;
    if (idx < n) {
        sdata[tid] = input[idx];
    }
    __syncthreads();
    for (int s = blockDim.x / 2; s > 0; s /= 2) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        atomicAdd(output, sdata[tid]);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = (n + kBlock - 1) / kBlock;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}