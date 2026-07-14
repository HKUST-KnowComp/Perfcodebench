#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kBlock = 256;

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[kBlock];

    int tid = threadIdx.x;
    int idx = blockIdx.x * kBlock + tid;
    int sum = 0;

    // grid-stride loop for coalesced, high-throughput reads
    for (int i = idx; i < n; i += gridDim.x * kBlock) {
        sum += input[i];
    }

    sdata[tid] = sum;
    __syncthreads();

    // tree reduction in shared memory
    for (int s = kBlock / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    int grid = (n + kBlock - 1) / kBlock;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        block_reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
