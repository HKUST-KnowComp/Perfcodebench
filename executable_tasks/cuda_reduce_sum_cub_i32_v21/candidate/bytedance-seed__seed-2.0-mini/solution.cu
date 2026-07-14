#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

    void* d_temp_storage = nullptr;
    if (temp_storage_bytes > 0) {
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    if (d_temp_storage != nullptr) {
        cudaFree(d_temp_storage);
    }
}