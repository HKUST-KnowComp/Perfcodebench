#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    size_t tmp_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, tmp_bytes, input, output, n);
    void* d_tmp = nullptr;
    cudaMalloc(&d_tmp, tmp_bytes);

    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_tmp, tmp_bytes, input, output, n);
    }

    cudaFree(d_tmp);
}