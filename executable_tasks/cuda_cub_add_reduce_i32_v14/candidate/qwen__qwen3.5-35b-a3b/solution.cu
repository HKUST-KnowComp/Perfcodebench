#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void partial_reduce_kernel(const int* input, int* temp, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int sum = 0;
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        sum += input[idx];
    }
    sdata[tid] = sum;
    __syncthreads();
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        temp[blockIdx.x] = sdata[0];
    }
}

__global__ void final_reduce_kernel(const int* temp, int* output, int n_temp) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int sum = 0;
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n_temp; idx += blockDim.x * gridDim.x) {
        sum += temp[idx];
    }
    sdata[tid] = sum;
    __syncthreads();
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        *output = sdata[0];
    }
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int temp_size = 1024;
    int* temp;
    cudaMalloc(&temp, temp_size * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        int blocks = 1024;
        int threads = 256;
        partial_reduce_kernel<<<blocks, threads, threads * sizeof(int)>>>(input, temp, n);
        final_reduce_kernel<<<1, temp_size, temp_size * sizeof(int)>>>(temp, output, temp_size);
    }

    cudaFree(temp);
}