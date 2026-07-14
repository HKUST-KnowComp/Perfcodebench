#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Allocate device buffers only once
    int* d_in  = nullptr;
    int* d_out = nullptr;
    cudaMalloc(&d_in,  n * sizeof(int));
    cudaMalloc(&d_out, n * sizeof(int));

    // Determine temporary storage size only once
    size_t temp_bytes = 0;
    cub::DeviceRadixSort::SortKeys(nullptr, temp_bytes,
                                   d_in, d_out, n);
    void* d_temp = nullptr;
    cudaMalloc(&d_temp, temp_bytes);

    // Copy input once; subsequent iterations reuse d_out as the new input
    cudaMemcpy(d_in, input, n * sizeof(int), cudaMemcpyDeviceToDevice);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceRadixSort::SortKeys(d_temp, temp_bytes,
                                       d_in, d_out, n);
        // Ping-pong buffers to avoid extra copy
        std::swap(d_in, d_out);
    }

    // After even number of swaps d_in is the final sorted data
    if (iters % 2 == 0) {
        cudaMemcpy(output, d_in, n * sizeof(int), cudaMemcpyDeviceToDevice);
    } else {
        cudaMemcpy(output, d_out, n * sizeof(int), cudaMemcpyDeviceToDevice);
    }

    cudaFree(d_in);
    cudaFree(d_out);
    cudaFree(d_temp);
}