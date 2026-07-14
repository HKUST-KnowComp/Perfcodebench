#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Calculate temporary storage requirement once before iterations
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

  // Allocate reusable temporary device storage
  void* d_temp_storage = nullptr;
  if (temp_storage_bytes > 0) {
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  // Run exclusive scan for each requested iteration
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Clean up temporary storage
  if (d_temp_storage != nullptr) {
    cudaFree(d_temp_storage);
  }
}