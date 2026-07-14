#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

#include <cstddef>
#include <cstdint>

namespace {

__global__ void copy_result(const int* from, int* to) {
    *to = *from;
}

} // anonymous namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    static_assert(sizeof(int) == sizeof(std::int32_t), "int must be 32-bit");

    // Device buffer for CUB reduction (single element)
    static int* d_out = nullptr;
    if (d_out == nullptr) {
        cudaMalloc(&d_out, sizeof(int));
    }

    // Temporary storage for CUB
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    if (d_temp_storage == nullptr) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes,
                               input, d_out, n);
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
    }

    for (int i = 0; i < iters; ++i) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes,
                               input, d_out, n);
    }

    // Copy result to the user-supplied pointer only once after all iterations
    copy_result<<<1, 1>>>(d_out, output);
    cudaDeviceSynchronize(); // Ensure the copy is complete before host returns
}
