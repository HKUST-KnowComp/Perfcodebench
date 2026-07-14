#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    if (n == 1) {
        cudaMemcpy(output, input, sizeof(int), cudaMemcpyDeviceToDevice);
        return;
    }

    static thread_local void* d_temp_storage = nullptr;
    static thread_local size_t temp_storage_bytes = 0;

    size_t required_bytes = 0;
    cub::DeviceRadixSort::SortKeys(nullptr, required_bytes, input, output, n);

    if (required_bytes > temp_storage_bytes) {
        cudaFree(d_temp_storage);
        cudaMalloc(&d_temp_storage, required_bytes);
        temp_storage_bytes = required_bytes;
    }

    for (int i = 0; i < iters; ++i) {
        cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, input, output, n);
    }
}
