#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kBlockDim = 256;
constexpr int kLoads    = 4;       // number of elements per thread

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int shared[];
    unsigned int tid = threadIdx.x;
    unsigned int start = blockIdx.x * (kBlockDim * kLoads) + tid * kLoads;
    unsigned int end   = start + kLoads;

    int sum = 0;
    #pragma unroll
    for (unsigned int i = start; i < end; ++i) {
        if (i < n) {
            sum += input[i];
        }
    }

    shared[tid] = sum;
    __syncthreads();

    // tree reduction in shared memory
    for (int s = kBlockDim / 2; s > 0; s >>= 1) {
        if (tid < s) {
            shared[tid] += shared[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, shared[0]);
    }
}

}  // anonymous namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Compute grid size to cover the whole array (with coarsening)
    int grid = (n + kBlockDim * kLoads - 1) / (kBlockDim * kLoads);
    const size_t shared_bytes = kBlockDim * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlockDim, shared_bytes>>>(input, output, n);
    }
}
