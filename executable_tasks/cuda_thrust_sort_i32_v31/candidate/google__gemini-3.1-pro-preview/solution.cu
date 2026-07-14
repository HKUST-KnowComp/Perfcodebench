#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

/**
 * Optimized integer sort using CUB DeviceRadixSort.
 * CUB provides a highly optimized parallel radix sort implementation for CUDA.
 */
void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements for CUB radix sort
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // First call to SortKeys with nullptr for temp storage determines the required size.
    // We use 'output' as both input and output for the sort to perform it in-place after the initial copy.
    cub::DeviceRadixSort::SortKeys(nullptr, temp_storage_bytes, output, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output buffer for each iteration to ensure correctness
        // and to allow in-place sorting on the output buffer.
        cudaMemcpy(output, input, sizeof(int) * n, cudaMemcpyDeviceToDevice);

        // Perform the parallel radix sort
        cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, output, output, n);
    }

    // Clean up temporary storage
    if (d_temp_storage) {
        cudaFree(d_temp_storage);
    }
}