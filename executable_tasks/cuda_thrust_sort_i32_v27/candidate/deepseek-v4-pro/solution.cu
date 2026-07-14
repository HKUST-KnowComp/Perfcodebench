#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    // Determine required temporary storage size for DeviceRadixSort
    size_t temp_bytes = 0;
    cub::DeviceRadixSort::SortKeys(nullptr, temp_bytes, output, output, n);
    void* d_temp = nullptr;
    if (temp_bytes > 0) {
        cudaMalloc(&d_temp, temp_bytes);
    }
    // Loop: copy input to output, then sort in-place
    for (int i = 0; i < iters; ++i) {
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDefault);
        cub::DeviceRadixSort::SortKeys(d_temp, temp_bytes, output, output, n);
    }
    // Ensure all work is done before returning
    cudaDeviceSynchronize();
    if (d_temp) cudaFree(d_temp);
}
