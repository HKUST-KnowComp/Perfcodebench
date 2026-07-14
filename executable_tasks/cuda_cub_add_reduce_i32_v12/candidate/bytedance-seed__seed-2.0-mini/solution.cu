#include "interface.h"
#include <cub/device/device_reduce.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        if (iters > 0) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    static int cached_n = 0;

    if (n != cached_n) {
        if (d_temp_storage != nullptr) {
            cudaFree(d_temp_storage);
            d_temp_storage = nullptr;
            temp_storage_bytes = 0;
        }

        cub::Sum<int64_t> reduce_op;
        const cudaError_t query_err = cub::DeviceReduce::Sum(
            nullptr, temp_storage_bytes, input, output, n, 0, reduce_op
        );
        if (query_err != cudaSuccess) {
            return;
        }

        const cudaError_t alloc_err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
        if (alloc_err != cudaSuccess) {
            d_temp_storage = nullptr;
            temp_storage_bytes = 0;
            return;
        }

        cached_n = n;
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum<int, int, cub::Sum<int64_t>>(
            d_temp_storage,
            temp_storage_bytes,
            input,
            output,
            n,
            0,
            cub::Sum<int64_t>()
        );
    }
}