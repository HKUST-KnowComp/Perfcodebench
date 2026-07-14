#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int* d_temp = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp, temp_storage_bytes);
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_storage_bytes, input, output, n);
    }
    cudaFree(d_temp);
}