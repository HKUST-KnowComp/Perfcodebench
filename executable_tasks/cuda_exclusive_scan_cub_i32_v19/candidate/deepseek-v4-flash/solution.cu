#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static int* d_temp = nullptr;
    static size_t temp_size = 0;
    static int cached_n = 0;

    if (n != cached_n) {
        if (d_temp) cudaFree(d_temp);
        temp_size = 0;
        d_temp = nullptr;
        cached_n = n;
    }

    if (n > 0) {
        if (temp_size == 0) {
            cub::DeviceScan::ExclusiveSum(nullptr, temp_size, input, output, n);
            cudaMalloc(&d_temp, temp_size);
        }
        for (int i = 0; i < iters; ++i) {
            cub::DeviceScan::ExclusiveSum(d_temp, temp_size, input, output, n);
        }
    } else {
        // n == 0, nothing to do
    }
}
