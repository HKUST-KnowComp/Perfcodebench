#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query the amount of temporary device storage required
  cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);

  // Allocate temporary storage once (if any is needed)
  if (temp_storage_bytes > 0) {
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  // Perform the exclusive scan repeatedly
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Free temporary storage
  if (d_temp_storage != nullptr) {
    cudaFree(d_temp_storage);
  }
}
