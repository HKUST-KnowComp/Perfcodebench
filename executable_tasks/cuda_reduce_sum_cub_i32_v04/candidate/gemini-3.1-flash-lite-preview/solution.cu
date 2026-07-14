#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* partial_sums, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * (blockDim.x * 2) + threadIdx.x;

    int sum = 0;
    if (i < n) sum = input[i];
    if (i + blockDim.x < n) sum += input[i + blockDim.x];

    sdata[tid] = sum;
    __syncthreads();

    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) partial_sums[blockIdx.x] = sdata[0];
}

__global__ void finalize_kernel(const int* partial_sums, int* output, int num_blocks) {
    int sum = 0;
    for (int i = 0; i < num_blocks; ++i) {
        sum += partial_sums[i];
    }
    *output = sum;
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        cudaMemset(output, 0, sizeof(int));
        return;
    }

    const int block_size = 256;
    const int grid_size = (n + (block_size * 2) - 1) / (block_size * 2);
    
    int* d_partial;
    cudaMalloc(&d_partial, grid_size * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<grid_size, block_size, block_size * sizeof(int)>>>(input, d_partial, n);
        finalize_kernel<<<1, 1>>>(d_partial, output, grid_size);
    }

    cudaFree(d_partial);
}