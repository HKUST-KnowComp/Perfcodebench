#include "interface.h"
#include <cuda_runtime.h>
#include <algorithm>

__global__ void reduce_block_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int sum = 0;
    int stride = blockDim.x * gridDim.x;
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += stride) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();

    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = std::min(1024, (n + kBlock - 1) / kBlock);
    size_t shared_mem = kBlock * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_block_kernel<<<grid, kBlock, shared_mem>>>(input, output, n);
    }
}