#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    static int cached_n = 0;

    // if n changed, free old allocation
    if (n != cached_n && d_temp_storage) {
        cudaFree(d_temp_storage);
        d_temp_storage = nullptr;
        temp_storage_bytes = 0;
    }
    if (d_temp_storage == nullptr) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
        cached_n = n;
    }

    cudaStream_t stream;
    cudaStreamCreate(&stream);

    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n, stream);
    }

    cudaStreamSynchronize(stream);
    cudaStreamDestroy(stream);
}
