#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop for input processing
    for (int i = idx; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    // Warp-level reduction
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Write warp results to shared memory
    if (tid % warpSize == 0) {
        sdata[tid / warpSize] = sum;
    }
    __syncthreads();

    // Final reduction in shared memory
    if (tid < (blockDim.x / warpSize)) {
        sum = sdata[tid];
        for (int offset = (blockDim.x / warpSize) / 2; offset > 0; offset /= 2) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
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