#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return;
    }

    static void* s_temp_storage = nullptr;
    static size_t s_temp_storage_bytes = 0;

    // Query required temporary storage size (no actual computation occurs here)
    size_t required_temp_bytes = 0;
    cub::DeviceScan::ExclusiveSum(
        nullptr,
        required_temp_bytes,
        input,
        output,
        n
    );

    // Reallocate temporary storage if size requirements change
    if (required_temp_bytes != s_temp_storage_bytes) {
        if (s_temp_storage != nullptr) {
            cudaFree(s_temp_storage);
            s_temp_storage = nullptr;
        }
        if (required_temp_bytes > 0) {
            const cudaError_t alloc_err = cudaMalloc(&s_temp_storage, required_temp_bytes);
            if (alloc_err != cudaSuccess) {
                return;
            }
            s_temp_storage_bytes = required_temp_bytes;
        } else {
            s_temp_storage_bytes = 0;
        }
    }

    // Execute exclusive scan for each requested iteration
    for (int iter = 0; iter < iters; ++iter) {
        const cudaError_t scan_err = cub::DeviceScan::ExclusiveSum(
            s_temp_storage,
            s_temp_storage_bytes,
            input,
            output,
            n
        );
        if (scan_err != cudaSuccess) {
            return;
        }
        // Catch any latent kernel launch errors
        const cudaError_t last_err = cudaGetLastError();
        if (last_err != cudaSuccess) {
            return;
        }
    }
}