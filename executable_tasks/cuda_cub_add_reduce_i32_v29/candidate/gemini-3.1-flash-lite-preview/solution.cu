#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2)
        sum += __shfl_down_sync(0xffffffff, sum, offset);

    if (tid % 32 == 0) sdata[tid / 32] = sum;
    __syncthreads();

    if (tid < 32) {
        sum = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
        for (int offset = 16; offset > 0; offset /= 2)
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        if (tid == 0) atomicAdd(output, sum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads = 256;
    const int blocks = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads, (threads / 32) * sizeof(int)>>>(input, output, n);
    }
}