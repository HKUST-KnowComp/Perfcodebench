#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * 2 + tid;
    
    int sum = 0;
    if (idx < n) sum += input[idx];
    if (idx + blockDim.x < n) sum += input[idx + blockDim.x];

    // Warp-level reduction
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
        sum += __shfl_down_sync(0xffffffff, sum, offset);

    if (tid % warpSize == 0) sdata[tid / warpSize] = sum;
    __syncthreads();

    // Final block reduction
    if (tid < (blockDim.x / warpSize)) {
        sum = sdata[tid];
        for (int offset = (blockDim.x / warpSize) / 2; offset > 0; offset /= 2)
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        if (tid == 0) atomicAdd(output, sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    const int grid_size = (n + (block_size * 2) - 1) / (block_size * 2);
    const int shared_mem = (block_size / 32) * sizeof(int);

    for (int i = 0; i < iters; ++i) {
        cudaMemset(output, 0, sizeof(int));
        if (n > 0) {
            reduce_kernel<<<grid_size, block_size, shared_mem>>>(input, output, n);
        }
    }
}