#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * 2 + tid;
    
    int sum = 0;
    if (idx < n) sum += input[idx];
    if (idx + blockDim.x < n) sum += input[idx + blockDim.x];

    // Warp-level reduction
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
        sum += __shfl_down_sync(0xffffffff, sum, offset);

    if (tid % warpSize == 0) sdata[tid / warpSize] = sum;
    __syncthreads();

    if (tid < blockDim.x / warpSize) {
        sum = sdata[tid];
        for (int offset = (blockDim.x / warpSize) / 2; offset > 0; offset /= 2)
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        if (tid == 0) atomicAdd(output, sum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads = 256;
    const int blocks = (n + (threads * 2) - 1) / (threads * 2);
    const int shared_mem = (threads / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads, shared_mem>>>(input, output, n);
    }
}