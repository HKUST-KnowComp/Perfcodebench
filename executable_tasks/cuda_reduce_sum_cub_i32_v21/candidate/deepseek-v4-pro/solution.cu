#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;
    int stride = blockDim.x * gridDim.x;
    int sum = 0;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();

    // Tree reduction
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
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
    const int grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}