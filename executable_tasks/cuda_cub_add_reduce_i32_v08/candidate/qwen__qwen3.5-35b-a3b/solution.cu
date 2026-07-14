#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void optimized_kernel(const int* input, int* output, int n) {
    int tid = threadIdx.x;
    int block_size = blockDim.x;
    int grid_size = gridDim.x;
    int stride = block_size * grid_size;

    int local_sum = 0;
    for (int idx = tid; idx < n; idx += stride) {
        local_sum += input[idx];
    }

    __shared__ int shared_sum[256];
    shared_sum[tid] = local_sum;
    __syncthreads();

    for (int offset = block_size / 2; offset > 0; offset /= 2) {
        if (tid < offset) {
            shared_sum[tid] += shared_sum[tid + offset];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, shared_sum[0]);
    }
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        optimized_kernel<<<1024, 256>>>(input, output, n);
    }
}