#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    size_t current_temp_bytes = 0;
    cub::DeviceRadixSort::SortKeys(nullptr, current_temp_bytes, input, output, n);
    if (temp_storage_bytes < current_temp_bytes) {
        if (d_temp_storage) cudaFree(d_temp_storage);
        cudaMalloc(&d_temp_storage, current_temp_bytes);
        temp_storage_bytes = current_temp_bytes;
    }
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, output, output, n);
    }
}
