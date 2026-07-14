#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void optimized_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to sum elements
    for (; idx < n; idx += blockDim.x * gridDim.x) {
        sum += input[idx];
    }

    sdata[tid] = sum;
    __syncthreads();

    // Block-level reduction
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    // If this is the first thread, atomically add to output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        optimized_kernel<<<1024, 256, 256 * sizeof(int)>>>(input, output, n);
    }
}