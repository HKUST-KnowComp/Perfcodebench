#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdint>
namespace {
__global__ void copy_to_output(const int* dev_sum, int* output) {
    atomicAdd(output, *dev_sum);
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int* d_out = nullptr;          // single int device buffer for CUB result
    cudaMalloc(&d_out, sizeof(int));

    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, d_out, n);
    void* d_temp = nullptr;
    cudaMalloc(&d_temp, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_storage_bytes, input, d_out, n);
        copy_to_output<<<1, 1>>>(d_out, output);
    }
    cudaFree(d_temp);
    cudaFree(d_out);
}