#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x * 2 + tid;
    int gridSize = blockDim.x * 2 * gridDim.x;

    int sum = 0;
    while (i < n) {
        sum += input[i];
        if (i + blockDim.x < n) sum += input[i + blockDim.x];
        i += gridSize;
    }

    sdata[tid] = sum;
    __syncthreads();

    if (blockDim.x >= 512) { if (tid < 256) sdata[tid] += sdata[tid + 256]; __syncthreads(); }
    if (blockDim.x >= 256) { if (tid < 128) sdata[tid] += sdata[tid + 128]; __syncthreads(); }
    if (blockDim.x >= 128) { if (tid < 64) sdata[tid] += sdata[tid + 64]; __syncthreads(); }

    if (tid < 32) {
        volatile int* vdata = sdata;
        if (blockDim.x >= 64) vdata[tid] += vdata[tid + 32];
        vdata[tid] += vdata[tid + 16];
        vdata[tid] += vdata[tid + 8];
        vdata[tid] += vdata[tid + 4];
        vdata[tid] += vdata[tid + 2];
        vdata[tid] += vdata[tid + 1];
    }

    if (tid == 0) atomicAdd(output, sdata[0]);
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int blockSize = 256;
    const int gridSize = 128;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_kernel<<<gridSize, blockSize, blockSize * sizeof(int)>>>(input, output, n);
    }
}