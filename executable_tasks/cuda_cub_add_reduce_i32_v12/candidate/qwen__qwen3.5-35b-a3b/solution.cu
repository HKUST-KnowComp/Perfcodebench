#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void optimized_kernel(const int* input, int* output, int n) {
    extern __shared__ int shared_mem[];
    if (threadIdx.x == 0) {
        shared_mem[0] = 0;
    }
    __syncthreads();
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int local_sum = 0;
    for (int i = idx; i < n; i += stride) {
        local_sum += input[i];
    }
    atomicAdd(&shared_mem[0], local_sum);
    __syncthreads();
    if (threadIdx.x == 0) {
        atomicAdd(output, shared_mem[0]);
    }
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        optimized_kernel<<<1024, 256, sizeof(int)>>>(input, output, n);
    }
}