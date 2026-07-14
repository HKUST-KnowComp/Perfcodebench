#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

static __global__ void reduce_tail(const int* partial, int* out) {
    int sum = 0;
    for (int i = threadIdx.x; i < 1024; i += blockDim.x)
        sum += partial[i];
    typedef cub::BlockReduce<int, 128> BlockReduce;
    __shared__ typename BlockReduce::TempStorage tmp;
    sum = BlockReduce(tmp).Sum(sum);
    if (threadIdx.x == 0) *out = sum;
}

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int threads = 128;
    const int blocks  = 1024;
    const size_t temp_bytes = blocks * sizeof(int);
    int* d_part;
    cudaMalloc(&d_part, temp_bytes);
    cudaMemsetAsync(output, 0, sizeof(int), nullptr);

    void* cub_temp = nullptr;
    size_t cub_bytes = 0;
    cub::DeviceReduce::Sum(cub_temp, cub_bytes, input, d_part, n);
    cudaMalloc(&cub_temp, cub_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(cub_temp, cub_bytes, input, d_part, n);
        reduce_tail<<<1, threads>>>(d_part, output);
    }
    cudaFree(cub_temp);
    cudaFree(d_part);
}