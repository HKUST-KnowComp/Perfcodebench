#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;
    int sum = 0;
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
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

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    constexpr int kGrid = 1024;
    size_t shared_mem = kBlock * sizeof(int);
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<kGrid, kBlock, shared_mem>>>(input, output, n);
    }
}