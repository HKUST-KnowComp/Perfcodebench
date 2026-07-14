#include "interface.h"
#include <cub/cub.cu>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Temporary storage for CUB scan
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to determine the required temporary storage size
    cub::DeviceScan::ExclusiveScan(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // Perform the exclusive scan
        cub::DeviceScan::ExclusiveScan(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    cudaFree(d_temp_storage);
}