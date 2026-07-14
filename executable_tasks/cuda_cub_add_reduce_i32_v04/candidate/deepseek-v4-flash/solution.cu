#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;

    // Use grid-stride loop to handle arbitrary n with fixed grid
    int sum = 0;
    for (int i = gid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    sdata[tid] = sum;
    __syncthreads();

    // Tree reduction in shared memory
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // First thread in block adds block sum to global output atomically
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) return;

    const int blockDim = 256;
    const int gridDim = 1024; // fixed grid size, keep same as baseline
    size_t sharedSize = blockDim * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<gridDim, blockDim, sharedSize>>>(input, output, n);
    }
}
