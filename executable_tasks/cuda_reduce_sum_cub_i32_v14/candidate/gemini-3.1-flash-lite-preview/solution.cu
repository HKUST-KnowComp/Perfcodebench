#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warpReduceSum(int val) {
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);
    return val;
}

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    extern __shared__ int shared[];
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    for (int i = gid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    sum = warpReduceSum(sum);

    if ((tid % warpSize) == 0) {
        shared[tid / warpSize] = sum;
    }
    __syncthreads();

    if (tid < (blockDim.x / warpSize)) {
        sum = shared[tid];
        sum = warpReduceSum(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int blockSize = 256;
    const int gridSize = 1024;
    const int sharedMemSize = (blockSize / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_kernel<<<gridSize, blockSize, sharedMemSize>>>(input, output, n);
    }
}