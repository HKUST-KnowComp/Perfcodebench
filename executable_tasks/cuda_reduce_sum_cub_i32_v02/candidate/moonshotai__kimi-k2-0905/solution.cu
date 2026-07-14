#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdint>

namespace {

constexpr int BLOCK = 256;

__global__ void final_reduce(const int* partial, int* out) {
    __shared__ int smem[BLOCK];
    int tid = threadIdx.x;
    int val = (tid < gridDim.x) ? partial[tid] : 0;
    smem[tid] = val;
    __syncthreads();

    for (int off = blockDim.x / 2; off > 0; off >>= 1) {
        if (tid < off) smem[tid] += smem[tid + off];
        __syncthreads();
    }
    if (tid == 0) *out = smem[0];
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    int* d_part;
    const int num_blocks = 1024;
    cudaMalloc(&d_part, num_blocks * sizeof(int));

    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_bytes, input, d_part, n);
    void* d_temp;
    cudaMalloc(&d_temp, temp_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, d_part, n);
        final_reduce<<<1, BLOCK>>>(d_part, output);
    }

    cudaFree(d_temp);
    cudaFree(d_part);
}