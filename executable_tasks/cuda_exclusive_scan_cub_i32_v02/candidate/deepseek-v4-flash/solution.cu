#include "interface.h"
#include <cub/cub.cuh>
#include <cstddef>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static void* d_temp = nullptr;
    static size_t temp_storage_bytes = 0;
    static int prev_n = 0;

    if (n != prev_n) {
        if (d_temp) {
            cudaFree(d_temp);
            d_temp = nullptr;
            temp_storage_bytes = 0;
        }
        if (n > 0) {
            cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
            cudaMalloc(&d_temp, temp_storage_bytes);
            prev_n = n;
        } else {
            prev_n = 0;
        }
    }

    for (int i = 0; i < iters; ++i) {
        if (n > 0) {
            cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
        }
    }
}