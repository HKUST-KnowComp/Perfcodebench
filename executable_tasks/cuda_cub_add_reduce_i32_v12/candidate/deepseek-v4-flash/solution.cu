#include "interface.h"
#include <cuda_runtime.h>

namespace {

template <int blockSize>
__global__ void __launch_bounds__(blockSize)
reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int gid = bid * blockDim.x + tid;

    // grid-stride loop to accumulate chunk
    int sum = 0;
    for (int i = gid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    sdata[tid] = sum;
    __syncthreads();

    // tree reduction in shared memory
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    const int blockSize = 256;
    const int gridSize = (n + blockSize - 1) / blockSize;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<blockSize><<<gridSize, blockSize, blockSize * sizeof(int)>>>(input, output, n);
    }
}
