#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[8];
    int tid = threadIdx.x;
    int warpId = tid >> 5;
    int laneId = tid & 31;

    int sum = 0;
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    if (laneId == 0) sdata[warpId] = sum;
    __syncthreads();

    if (warpId == 0) {
        int blockSum = (laneId < 8) ? sdata[laneId] : 0;
        #pragma unroll
        for (int offset = 4; offset > 0; offset >>= 1) {
            blockSum += __shfl_down_sync(0xffffffff, blockSum, offset);
        }
        if (tid == 0) atomicAdd(output, blockSum);
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