#include "interface.h"
#include <cuda_runtime.h>

namespace {
__device__ __forceinline__ int warp_reduce_sum(int val) {
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * 256 + threadIdx.x;
    unsigned int stride = 256 * gridDim.x;

    int sum = 0;
    for (unsigned int i = idx; i < n; i += stride) {
        sum += input[i];
    }

    sum = warp_reduce_sum(sum);
    if (tid % 32 == 0) {
        sdata[tid / 32] = sum;
    }
    __syncthreads();

    if (tid < 32) {
        int val = (tid < 8) ? sdata[tid] : 0;
        val = warp_reduce_sum(val);
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads = 256;
    int blocks = (n + threads - 1) / threads;
    if (blocks > 1024) blocks = 1024;
    const size_t shared_mem = threads * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads, shared_mem>>>(input, output, n);
    }
}
