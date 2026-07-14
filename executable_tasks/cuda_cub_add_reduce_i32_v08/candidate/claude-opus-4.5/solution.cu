#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    static int last_n = 0;

    // Determine temp storage requirements if n changed or first call
    if (n != last_n || d_temp_storage == nullptr) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
            d_temp_storage = nullptr;
        }
        temp_storage_bytes = 0;
        cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
        last_n = n;
    }

    for (int iter = 0; iter < iters; ++iter) {
        size_t bytes = temp_storage_bytes;
        cub::DeviceReduce::Sum(d_temp_storage, bytes, input, output, n);
    }
}
