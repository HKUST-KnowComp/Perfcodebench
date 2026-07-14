#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
    int sum = 0;
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    if ((threadIdx.x & 31) == 0) {
        atomicAdd(output, sum);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int blocks = (n + 255) / 256;
    if (blocks > 1024) blocks = 1024;
    if (blocks < 1) blocks = 1;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<blocks, 256>>>(input, output, n);
    }
}