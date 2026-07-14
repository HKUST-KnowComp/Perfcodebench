#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    // Query temporary storage requirements
    size_t temp_storage_bytes = 0;
    cub::DeviceRadixSort::SortKeys(nullptr, temp_storage_bytes, input, output, n);
    // Allocate temporary storage once for all iterations
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    // Perform iters out-of-place sorts
    for (int i = 0; i < iters; ++i) {
        cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    // Free temporary storage
    cudaFree(d_temp_storage);
}