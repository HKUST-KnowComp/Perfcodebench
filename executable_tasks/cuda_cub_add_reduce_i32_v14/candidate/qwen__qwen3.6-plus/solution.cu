#include "interface.h"
#include <cuda_runtime.h>

namespace {
template <unsigned int blockSize>
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockSize * 2 + threadIdx.x;
    unsigned int gridSize = blockSize * 2 * gridDim.x;
    int sum = 0;

    while (i < n) {
        sum += input[i];
        if (i + blockSize < n) {
            sum += input[i + blockSize];
        }
        i += gridSize;
    }

    sdata[tid] = sum;
    __syncthreads();

    for (unsigned int s = blockSize / 2; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid < 32) {
        int val = sdata[tid];
        val += __shfl_down_sync(0xffffffff, val, 16);
        val += __shfl_down_sync(0xffffffff, val, 8);
        val += __shfl_down_sync(0xffffffff, val, 4);
        val += __shfl_down_sync(0xffffffff, val, 2);
        val += __shfl_down_sync(0xffffffff, val, 1);
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    int blocks = (n + 511) / 512;
    if (blocks > 2048) blocks = 2048;
    if (blocks < 1) blocks = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<256><<<blocks, 256, 256 * sizeof(int)>>>(input, output, n);
    }
}