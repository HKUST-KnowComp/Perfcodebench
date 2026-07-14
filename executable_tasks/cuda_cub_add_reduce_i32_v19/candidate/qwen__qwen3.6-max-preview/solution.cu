#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void __launch_bounds__(256) reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    __shared__ int sdata[32];
    int lane = threadIdx.x & 31;
    int wid = threadIdx.x >> 5;
    if (lane == 0) sdata[wid] = sum;
    __syncthreads();
    if (wid == 0) {
        sum = (lane < (blockDim.x >> 5)) ? sdata[lane] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        if (lane == 0) atomicAdd(output, sum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    if (blocks > 1024) blocks = 1024;
    if (blocks == 0) blocks = 1;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, threads>>>(input, output, n);
    }
}