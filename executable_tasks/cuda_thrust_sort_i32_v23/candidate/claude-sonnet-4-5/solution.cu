#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Determine temporary storage requirements
    cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    // Perform sorting iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    // Free temporary storage
    cudaFree(d_temp_storage);
}