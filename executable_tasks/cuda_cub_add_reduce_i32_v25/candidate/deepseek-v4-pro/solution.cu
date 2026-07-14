#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    static void* d_temp_storage = nullptr;
    static size_t temp_bytes = 0;

    // Reallocate temporary storage if needed
    size_t required_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, required_bytes, input, output, n);
    if (required_bytes != temp_bytes) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
            d_temp_storage = nullptr;
            temp_bytes = 0;
        }
        if (required_bytes > 0) {
            cudaMalloc(&d_temp_storage, required_bytes);
            temp_bytes = required_bytes;
        }
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_bytes, input, output, n);
    }
}
