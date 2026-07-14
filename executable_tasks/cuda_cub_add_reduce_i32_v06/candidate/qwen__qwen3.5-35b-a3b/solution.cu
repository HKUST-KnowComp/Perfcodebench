#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_block_kernel(const int* input, int* block_sums, int n) {
    extern __shared__ int shared[];
    int tid = threadIdx.x;
    int sum = 0;
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        sum += input[idx];
    }
    shared[tid] = sum;
    __syncthreads();
    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (tid < stride) {
            shared[tid] += shared[tid + stride];
        }
        __syncthreads();
    }
    if (tid == 0) {
        block_sums[blockIdx.x] = shared[0];
    }
}

__global__ void reduce_blocks_kernel(const int* block_sums, int* output, int num_blocks) {
    extern __shared__ int shared[];
    int tid = threadIdx.x;
    int sum = 0;
    for (int idx = tid; idx < num_blocks; idx += blockDim.x) {
        sum += block_sums[idx];
    }
    shared[tid] = sum;
    __syncthreads();
    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (tid < stride) {
            shared[tid] += shared[tid + stride];
        }
        __syncthreads();
    }
    if (tid == 0) {
        *output = shared[0];
    }
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int num_blocks = 1024;
    int* block_sums;
    cudaMalloc(&block_sums, num_blocks * sizeof(int));
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cudaMemset(block_sums, 0, num_blocks * sizeof(int));
        reduce_block_kernel<<<num_blocks, 256, 256 * sizeof(int)>>>(input, block_sums, n);
        reduce_blocks_kernel<<<1, num_blocks>>>(block_sums, output, num_blocks);
    }
    cudaFree(block_sums);
}